#include "bsd/store/globals.hh"
#include "bsd/expr/print-ambiguous.hh"
#include "bsd/main/shared.hh"
#include "bsd/expr/eval.hh"
#include "bsd/expr/eval-inline.hh"
#include "bsd/expr/get-drvs.hh"
#include "bsd/expr/attr-path.hh"
#include "bsd/util/signals.hh"
#include "bsd/expr/value-to-xml.hh"
#include "bsd/expr/value-to-json.hh"
#include "bsd/store/store-open.hh"
#include "bsd/store/local-fs-store.hh"
#include "bsd/cmd/common-eval-args.hh"
#include "bsd/cmd/legacy.hh"
#include "man-pages.hh"

#include <map>
#include <iostream>


using namespace bsd;


static Path gcRoot;
static int rootNr = 0;


enum OutputKind { okPlain, okRaw, okXML, okJSON };

void processExpr(EvalState & state, const Strings & attrPaths,
    bool parseOnly, bool strict, Bindings & autoArgs,
    bool evalOnly, OutputKind output, bool location, Expr * e)
{
    if (parseOnly) {
        e->show(state.symbols, std::cout);
        std::cout << "\n";
        return;
    }

    Value vRoot;
    state.eval(e, vRoot);

    for (auto & i : attrPaths) {
        Value & v(*findAlongAttrPath(state, i, autoArgs, vRoot).first);
        state.forceValue(v, v.determinePos(noPos));

        BsdStringContext context;
        if (evalOnly) {
            Value vRes;
            if (autoArgs.empty())
                vRes = v;
            else
                state.autoCallFunction(autoArgs, v, vRes);
            if (output == okRaw)
                std::cout << *state.coerceToString(noPos, vRes, context, "while generating the bsd-instantiate output");
                // We intentionally don't output a newline here. The default PS1 for Bash in BasedLinux starts with a newline
                // and other interactive shells like Zsh are smart enough to print a missing newline before the prompt.
            else if (output == okXML)
                printValueAsXML(state, strict, location, vRes, std::cout, context, noPos);
            else if (output == okJSON) {
                printValueAsJSON(state, strict, vRes, v.determinePos(noPos), std::cout, context);
                std::cout << std::endl;
            } else {
                if (strict) state.forceValueDeep(vRes);
                std::set<const void *> seen;
                printAmbiguous(vRes, state.symbols, std::cout, &seen, std::numeric_limits<int>::max());
                std::cout << std::endl;
            }
        } else {
            PackageInfos drvs;
            getDerivations(state, v, "", autoArgs, drvs, false);
            for (auto & i : drvs) {
                auto drvPath = i.requireDrvPath();
                auto drvPathS = state.store->printStorePath(drvPath);

                /* What output do we want? */
                std::string outputName = i.queryOutputName();
                if (outputName == "")
                    throw Error("derivation '%1%' lacks an 'outputName' attribute", drvPathS);

                if (gcRoot == "")
                    printGCWarning();
                else {
                    Path rootName = absPath(gcRoot);
                    if (++rootNr > 1) rootName += "-" + std::to_string(rootNr);
                    auto store2 = state.store.dynamic_pointer_cast<LocalFSStore>();
                    if (store2)
                        drvPathS = store2->addPermRoot(drvPath, rootName);
                }
                std::cout << fmt("%s%s\n", drvPathS, (outputName != "out" ? "!" + outputName : ""));
            }
        }
    }
}


static int main_bsd_instantiate(int argc, char * * argv)
{
    {
        Strings files;
        bool readStdin = false;
        bool fromArgs = false;
        bool findFile = false;
        bool evalOnly = false;
        bool parseOnly = false;
        OutputKind outputKind = okPlain;
        bool xmlOutputSourceLocation = true;
        bool strict = false;
        Strings attrPaths;
        bool wantsReadWrite = false;

        struct MyArgs : LegacyArgs, MixEvalArgs
        {
            using LegacyArgs::LegacyArgs;
        };

        MyArgs myArgs(std::string(baseNameOf(argv[0])), [&](Strings::iterator & arg, const Strings::iterator & end) {
            if (*arg == "--help")
                showManPage("bsd-instantiate");
            else if (*arg == "--version")
                printVersion("bsd-instantiate");
            else if (*arg == "-")
                readStdin = true;
            else if (*arg == "--expr" || *arg == "-E")
                fromArgs = true;
            else if (*arg == "--eval" || *arg == "--eval-only")
                evalOnly = true;
            else if (*arg == "--read-write-mode")
                wantsReadWrite = true;
            else if (*arg == "--parse" || *arg == "--parse-only")
                parseOnly = evalOnly = true;
            else if (*arg == "--find-file")
                findFile = true;
            else if (*arg == "--attr" || *arg == "-A")
                attrPaths.push_back(getArg(*arg, arg, end));
            else if (*arg == "--add-root")
                gcRoot = getArg(*arg, arg, end);
            else if (*arg == "--indirect")
                ;
            else if (*arg == "--raw")
                outputKind = okRaw;
            else if (*arg == "--xml")
                outputKind = okXML;
            else if (*arg == "--json")
                outputKind = okJSON;
            else if (*arg == "--no-location")
                xmlOutputSourceLocation = false;
            else if (*arg == "--strict")
                strict = true;
            else if (*arg == "--dry-run")
                settings.readOnlyMode = true;
            else if (*arg != "" && arg->at(0) == '-')
                return false;
            else
                files.push_back(*arg);
            return true;
        });

        myArgs.parseCmdline(argvToStrings(argc, argv));

        if (evalOnly && !wantsReadWrite)
            settings.readOnlyMode = true;

        auto store = openStore();
        auto evalStore = myArgs.evalStoreUrl ? openStore(*myArgs.evalStoreUrl) : store;

        auto state = std::make_unique<EvalState>(myArgs.lookupPath, evalStore, fetchSettings, evalSettings, store);
        state->repair = myArgs.repair;

        Bindings & autoArgs = *myArgs.getAutoArgs(*state);

        if (attrPaths.empty()) attrPaths = {""};

        if (findFile) {
            for (auto & i : files) {
                auto p = state->findFile(i);
                if (auto fn = p.getPhysicalPath())
                    std::cout << fn->string() << std::endl;
                else
                    throw Error("'%s' has no physical path", p);
            }
            return 0;
        }

        if (readStdin) {
            Expr * e = state->parseStdin();
            processExpr(*state, attrPaths, parseOnly, strict, autoArgs,
                evalOnly, outputKind, xmlOutputSourceLocation, e);
        } else if (files.empty() && !fromArgs)
            files.push_back("./default.bsd");

        for (auto & i : files) {
            Expr * e = fromArgs
                ? state->parseExprFromString(i, state->rootPath("."))
                : state->parseExprFromFile(resolveExprPath(lookupFileArg(*state, i)));
            processExpr(*state, attrPaths, parseOnly, strict, autoArgs,
                evalOnly, outputKind, xmlOutputSourceLocation, e);
        }

        state->maybePrintStats();

        return 0;
    }
}

static RegisterLegacyCommand r_bsd_instantiate("bsd-instantiate", main_bsd_instantiate);
