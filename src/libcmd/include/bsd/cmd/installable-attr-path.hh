#pragma once
///@file

#include "bsd/store/globals.hh"
#include "bsd/cmd/installable-value.hh"
#include "bsd/store/outputs-spec.hh"
#include "bsd/cmd/command.hh"
#include "bsd/expr/attr-path.hh"
#include "bsd/cmd/common-eval-args.hh"
#include "bsd/store/derivations.hh"
#include "bsd/expr/eval-inline.hh"
#include "bsd/expr/eval.hh"
#include "bsd/expr/get-drvs.hh"
#include "bsd/store/store-api.hh"
#include "bsd/main/shared.hh"
#include "bsd/expr/eval-cache.hh"
#include "bsd/util/url.hh"
#include "bsd/fetchers/registry.hh"
#include "bsd/store/build-result.hh"

#include <regex>
#include <queue>

#include <nlohmann/json.hpp>

namespace bsd {

class InstallableAttrPath : public InstallableValue
{
    SourceExprCommand & cmd;
    RootValue v;
    std::string attrPath;
    ExtendedOutputsSpec extendedOutputsSpec;

    InstallableAttrPath(
        ref<EvalState> state,
        SourceExprCommand & cmd,
        Value * v,
        const std::string & attrPath,
        ExtendedOutputsSpec extendedOutputsSpec);

    std::string what() const override { return attrPath; };

    std::pair<Value *, PosIdx> toValue(EvalState & state) override;

    DerivedPathsWithInfo toDerivedPaths() override;

public:

    static InstallableAttrPath parse(
        ref<EvalState> state,
        SourceExprCommand & cmd,
        Value * v,
        std::string_view prefix,
        ExtendedOutputsSpec extendedOutputsSpec);
};

}
