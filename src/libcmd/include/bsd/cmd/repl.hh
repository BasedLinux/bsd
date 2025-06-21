#pragma once
///@file

#include "bsd/expr/eval.hh"

namespace bsd {

struct AbstractBsdRepl
{
    ref<EvalState> state;
    Bindings * autoArgs;

    AbstractBsdRepl(ref<EvalState> state)
        : state(state)
    { }

    virtual ~AbstractBsdRepl()
    { }

    typedef std::vector<std::pair<Value*,std::string>> AnnotatedValues;

    using RunBsd = void(Path program, const Strings & args, const std::optional<std::string> & input);

    /**
     * @param runBsd Function to run the bsd CLI to support various
     * `:<something>` commands. Optional; if not provided,
     * everything else will still work fine, but those commands won't.
     */
    static std::unique_ptr<AbstractBsdRepl> create(
        const LookupPath & lookupPath,
        bsd::ref<Store> store,
        ref<EvalState> state,
        std::function<AnnotatedValues()> getValues,
        RunBsd * runBsd = nullptr);

    static ReplExitStatus runSimple(
        ref<EvalState> evalState,
        const ValMap & extraEnv);

    virtual void initEnv() = 0;

    virtual ReplExitStatus mainLoop() = 0;
};

}
