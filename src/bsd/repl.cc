#include "bsd/expr/eval.hh"
#include "bsd/expr/eval-settings.hh"
#include "bsd/util/config-global.hh"
#include "bsd/store/globals.hh"
#include "bsd/store/store-open.hh"
#include "bsd/cmd/command.hh"
#include "bsd/cmd/installable-value.hh"
#include "bsd/cmd/repl.hh"
#include "bsd/util/processes.hh"
#include "self-exe.hh"

namespace bsd {

void runBsd(Path program, const Strings & args,
    const std::optional<std::string> & input = {})
{
    auto subprocessEnv = getEnv();
    subprocessEnv["NIX_CONFIG"] = globalConfig.toKeyValue();
    //isInteractive avoid grabling interactive commands
    runProgram2(RunOptions {
        .program = getBsdBin(program).string(),
        .args = args,
        .environment = subprocessEnv,
        .input = input,
        .isInteractive = true,
    });

    return;
}

struct CmdRepl : RawInstallablesCommand
{
    CmdRepl() {
        evalSettings.pureEval = false;
    }

    /**
     * This command is stable before the others
     */
    std::optional<ExperimentalFeature> experimentalFeature() override
    {
        return std::nullopt;
    }

    std::vector<std::string> files;

    Strings getDefaultFlakeAttrPaths() override
    {
        return {""};
    }

    bool forceImpureByDefault() override
    {
        return true;
    }

    std::string description() override
    {
        return "start an interactive environment for evaluating Bsd expressions";
    }

    std::string doc() override
    {
        return
          #include "repl.md"
          ;
    }

    void applyDefaultInstallables(std::vector<std::string> & rawInstallables) override
    {
        if (rawInstallables.empty() && (file.has_value() || expr.has_value())) {
            rawInstallables.push_back(".");
        }
    }

    void run(ref<Store> store, std::vector<std::string> && rawInstallables) override
    {
        auto state = getEvalState();
        auto getValues = [&]()->AbstractBsdRepl::AnnotatedValues{
            auto installables = parseInstallables(store, rawInstallables);
            AbstractBsdRepl::AnnotatedValues values;
            for (auto & installable_: installables){
                auto & installable = InstallableValue::require(*installable_);
                auto what = installable.what();
                if (file){
                    auto [val, pos] = installable.toValue(*state);
                    auto what = installable.what();
                    state->forceValue(*val, pos);
                    auto autoArgs = getAutoArgs(*state);
                    auto valPost = state->allocValue();
                    state->autoCallFunction(*autoArgs, *val, *valPost);
                    state->forceValue(*valPost, pos);
                    values.push_back( {valPost, what });
                } else {
                    auto [val, pos] = installable.toValue(*state);
                    values.push_back( {val, what} );
                }
            }
            return values;
        };
        auto repl = AbstractBsdRepl::create(
            lookupPath,
            openStore(),
            state,
            getValues,
            runBsd
        );
        repl->autoArgs = getAutoArgs(*repl->state);
        repl->initEnv();
        repl->mainLoop();
    }
};

static auto rCmdRepl = registerCommand<CmdRepl>("repl");

}
