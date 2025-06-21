#include "bsd/store/globals.hh"
#include "bsd/cmd/installable-attr-path.hh"
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

InstallableAttrPath::InstallableAttrPath(
    ref<EvalState> state,
    SourceExprCommand & cmd,
    Value * v,
    const std::string & attrPath,
    ExtendedOutputsSpec extendedOutputsSpec)
    : InstallableValue(state)
    , cmd(cmd)
    , v(allocRootValue(v))
    , attrPath(attrPath)
    , extendedOutputsSpec(std::move(extendedOutputsSpec))
{ }

std::pair<Value *, PosIdx> InstallableAttrPath::toValue(EvalState & state)
{
    auto [vRes, pos] = findAlongAttrPath(state, attrPath, *cmd.getAutoArgs(state), **v);
    state.forceValue(*vRes, pos);
    return {vRes, pos};
}

DerivedPathsWithInfo InstallableAttrPath::toDerivedPaths()
{
    auto [v, pos] = toValue(*state);

    if (std::optional derivedPathWithInfo = trySinglePathToDerivedPaths(
        *v,
        pos,
        fmt("while evaluating the attribute '%s'", attrPath)))
    {
        return { *derivedPathWithInfo };
    }

    Bindings & autoArgs = *cmd.getAutoArgs(*state);

    PackageInfos packageInfos;
    getDerivations(*state, *v, "", autoArgs, packageInfos, false);

    // Backward compatibility hack: group results by drvPath. This
    // helps keep .all output together.
    std::map<StorePath, OutputsSpec> byDrvPath;

    for (auto & packageInfo : packageInfos) {
        auto drvPath = packageInfo.queryDrvPath();
        if (!drvPath)
            throw Error("'%s' is not a derivation", what());

        auto newOutputs = std::visit(overloaded {
            [&](const ExtendedOutputsSpec::Default & d) -> OutputsSpec {
                StringSet outputsToInstall;
                for (auto & output : packageInfo.queryOutputs(false, true))
                    outputsToInstall.insert(output.first);
                if (outputsToInstall.empty())
                    outputsToInstall.insert("out");
                return OutputsSpec::Names { std::move(outputsToInstall) };
            },
            [&](const ExtendedOutputsSpec::Explicit & e) -> OutputsSpec {
                return e;
            },
        }, extendedOutputsSpec.raw);

        auto [iter, didInsert] = byDrvPath.emplace(*drvPath, newOutputs);

        if (!didInsert)
            iter->second = iter->second.union_(newOutputs);
    }

    DerivedPathsWithInfo res;
    for (auto & [drvPath, outputs] : byDrvPath)
        res.push_back({
            .path = DerivedPath::Built {
                .drvPath = makeConstantStorePathRef(drvPath),
                .outputs = outputs,
            },
            .info = make_ref<ExtraPathInfoValue>(ExtraPathInfoValue::Value {
                .extendedOutputsSpec = outputs,
                /* FIXME: reconsider backwards compatibility above
                   so we can fill in this info. */
            }),
        });

    return res;
}

InstallableAttrPath InstallableAttrPath::parse(
    ref<EvalState> state,
    SourceExprCommand & cmd,
    Value * v,
    std::string_view prefix,
    ExtendedOutputsSpec extendedOutputsSpec)
{
    return {
        state, cmd, v,
        prefix == "." ? "" : std::string { prefix },
        std::move(extendedOutputsSpec),
    };
}

}
