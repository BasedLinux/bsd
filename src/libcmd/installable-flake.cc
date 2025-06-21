#include "bsd/store/globals.hh"
#include "bsd/cmd/installable-flake.hh"
#include "bsd/cmd/installable-derived-path.hh"
#include "bsd/store/outputs-spec.hh"
#include "bsd/util/util.hh"
#include "bsd/cmd/command.hh"
#include "bsd/expr/attr-path.hh"
#include "bsd/cmd/common-eval-args.hh"
#include "bsd/store/derivations.hh"
#include "bsd/expr/eval-inline.hh"
#include "bsd/expr/eval.hh"
#include "bsd/expr/get-drvs.hh"
#include "bsd/store/store-api.hh"
#include "bsd/main/shared.hh"
#include "bsd/flake/flake.hh"
#include "bsd/expr/eval-cache.hh"
#include "bsd/util/url.hh"
#include "bsd/fetchers/registry.hh"
#include "bsd/store/build-result.hh"

#include <regex>
#include <queue>

#include <nlohmann/json.hpp>

namespace bsd {

std::vector<std::string> InstallableFlake::getActualAttrPaths()
{
    std::vector<std::string> res;
    if (attrPaths.size() == 1 && attrPaths.front().starts_with(".")){
        attrPaths.front().erase(0,1);
        res.push_back(attrPaths.front());
        return res;
    }

    for (auto & prefix : prefixes)
        res.push_back(prefix + *attrPaths.begin());

    for (auto & s : attrPaths)
        res.push_back(s);

    return res;
}

static std::string showAttrPaths(const std::vector<std::string> & paths)
{
    std::string s;
    for (const auto & [n, i] : enumerate(paths)) {
        if (n > 0) s += n + 1 == paths.size() ? " or " : ", ";
        s += '\''; s += i; s += '\'';
    }
    return s;
}

InstallableFlake::InstallableFlake(
    SourceExprCommand * cmd,
    ref<EvalState> state,
    FlakeRef && flakeRef,
    std::string_view fragment,
    ExtendedOutputsSpec extendedOutputsSpec,
    Strings attrPaths,
    Strings prefixes,
    const flake::LockFlags & lockFlags)
    : InstallableValue(state),
      flakeRef(flakeRef),
      attrPaths(fragment == "" ? attrPaths : Strings{(std::string) fragment}),
      prefixes(fragment == "" ? Strings{} : prefixes),
      extendedOutputsSpec(std::move(extendedOutputsSpec)),
      lockFlags(lockFlags)
{
    if (cmd && cmd->getAutoArgs(*state)->size())
        throw UsageError("'--arg' and '--argstr' are incompatible with flakes");
}

DerivedPathsWithInfo InstallableFlake::toDerivedPaths()
{
    Activity act(*logger, lvlTalkative, actUnknown, fmt("evaluating derivation '%s'", what()));

    auto attr = getCursor(*state);

    auto attrPath = attr->getAttrPathStr();

    if (!attr->isDerivation()) {

        // FIXME: use eval cache?
        auto v = attr->forceValue();

        if (std::optional derivedPathWithInfo = trySinglePathToDerivedPaths(
            v,
            noPos,
            fmt("while evaluating the flake output attribute '%s'", attrPath)))
        {
            return { *derivedPathWithInfo };
        } else {
            throw Error(
                "expected flake output attribute '%s' to be a derivation or path but found %s: %s",
                attrPath,
                showType(v),
                ValuePrinter(*this->state, v, errorPrintOptions)
            );
        }
    }

    auto drvPath = attr->forceDerivation();

    std::optional<BsdInt::Inner> priority;

    if (attr->maybeGetAttr(state->sOutputSpecified)) {
    } else if (auto aMeta = attr->maybeGetAttr(state->sMeta)) {
        if (auto aPriority = aMeta->maybeGetAttr("priority"))
            priority = aPriority->getInt().value;
    }

    return {{
        .path = DerivedPath::Built {
            .drvPath = makeConstantStorePathRef(std::move(drvPath)),
            .outputs = std::visit(overloaded {
                [&](const ExtendedOutputsSpec::Default & d) -> OutputsSpec {
                    StringSet outputsToInstall;
                    if (auto aOutputSpecified = attr->maybeGetAttr(state->sOutputSpecified)) {
                        if (aOutputSpecified->getBool()) {
                            if (auto aOutputName = attr->maybeGetAttr("outputName"))
                                outputsToInstall = { aOutputName->getString() };
                        }
                    } else if (auto aMeta = attr->maybeGetAttr(state->sMeta)) {
                        if (auto aOutputsToInstall = aMeta->maybeGetAttr("outputsToInstall"))
                            for (auto & s : aOutputsToInstall->getListOfStrings())
                                outputsToInstall.insert(s);
                    }

                    if (outputsToInstall.empty())
                        outputsToInstall.insert("out");

                    return OutputsSpec::Names { std::move(outputsToInstall) };
                },
                [&](const ExtendedOutputsSpec::Explicit & e) -> OutputsSpec {
                    return e;
                },
            }, extendedOutputsSpec.raw),
        },
        .info = make_ref<ExtraPathInfoFlake>(
            ExtraPathInfoValue::Value {
                .priority = priority,
                .attrPath = attrPath,
                .extendedOutputsSpec = extendedOutputsSpec,
            },
            ExtraPathInfoFlake::Flake {
                .originalRef = flakeRef,
                .lockedRef = getLockedFlake()->flake.lockedRef,
            }),
    }};
}

std::pair<Value *, PosIdx> InstallableFlake::toValue(EvalState & state)
{
    return {&getCursor(state)->forceValue(), noPos};
}

std::vector<ref<eval_cache::AttrCursor>>
InstallableFlake::getCursors(EvalState & state)
{
    auto evalCache = openEvalCache(state, getLockedFlake());

    auto root = evalCache->getRoot();

    std::vector<ref<eval_cache::AttrCursor>> res;

    Suggestions suggestions;
    auto attrPaths = getActualAttrPaths();

    for (auto & attrPath : attrPaths) {
        debug("trying flake output attribute '%s'", attrPath);

        auto attr = root->findAlongAttrPath(parseAttrPath(state, attrPath));
        if (attr) {
            res.push_back(ref(*attr));
        } else {
            suggestions += attr.getSuggestions();
        }
    }

    if (res.size() == 0)
        throw Error(
            suggestions,
            "flake '%s' does not provide attribute %s",
            flakeRef,
            showAttrPaths(attrPaths));

    return res;
}

std::shared_ptr<flake::LockedFlake> InstallableFlake::getLockedFlake() const
{
    if (!_lockedFlake) {
        flake::LockFlags lockFlagsApplyConfig = lockFlags;
        // FIXME why this side effect?
        lockFlagsApplyConfig.applyBsdConfig = true;
        _lockedFlake = std::make_shared<flake::LockedFlake>(lockFlake(
            flakeSettings, *state, flakeRef, lockFlagsApplyConfig));
    }
    return _lockedFlake;
}

FlakeRef InstallableFlake::bsdpkgsFlakeRef() const
{
    auto lockedFlake = getLockedFlake();

    if (auto bsdpkgsInput = lockedFlake->lockFile.findInput({"bsdpkgs"})) {
        if (auto lockedNode = std::dynamic_pointer_cast<const flake::LockedNode>(bsdpkgsInput)) {
            debug("using bsdpkgs flake '%s'", lockedNode->lockedRef);
            return std::move(lockedNode->lockedRef);
        }
    }

    return defaultBsdpkgsFlakeRef();
}

}
