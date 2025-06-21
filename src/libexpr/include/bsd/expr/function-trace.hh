#pragma once
///@file

#include "bsd/expr/eval.hh"
#include "bsd/expr/eval-profiler.hh"

namespace bsd {

class FunctionCallTrace : public EvalProfiler
{
    Hooks getNeededHooksImpl() const override
    {
        return Hooks().set(preFunctionCall).set(postFunctionCall);
    }

public:
    FunctionCallTrace() = default;

    [[gnu::noinline]] void
    preFunctionCallHook(EvalState & state, const Value & v, std::span<Value *> args, const PosIdx pos) override;
    [[gnu::noinline]] void
    postFunctionCallHook(EvalState & state, const Value & v, std::span<Value *> args, const PosIdx pos) override;
};

}
