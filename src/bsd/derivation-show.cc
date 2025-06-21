// FIXME: integrate this with `bsd path-info`?
// FIXME: rename to 'bsd store derivation show'?

#include "bsd/cmd/command.hh"
#include "bsd/main/common-args.hh"
#include "bsd/store/store-api.hh"
#include "bsd/util/archive.hh"
#include "bsd/store/derivations.hh"
#include <nlohmann/json.hpp>

using namespace bsd;
using json = nlohmann::json;

struct CmdShowDerivation : InstallablesCommand, MixPrintJSON
{
    bool recursive = false;

    CmdShowDerivation()
    {
        addFlag({
            .longName = "recursive",
            .shortName = 'r',
            .description = "Include the dependencies of the specified derivations.",
            .handler = {&recursive, true},
        });
    }

    std::string description() override
    {
        return "show the contents of a store derivation";
    }

    std::string doc() override
    {
        return
          #include "derivation-show.md"
          ;
    }

    Category category() override { return catUtility; }

    void run(ref<Store> store, Installables && installables) override
    {
        auto drvPaths = Installable::toDerivations(store, installables, true);

        if (recursive) {
            StorePathSet closure;
            store->computeFSClosure(drvPaths, closure);
            drvPaths = std::move(closure);
        }

        json jsonRoot = json::object();

        for (auto & drvPath : drvPaths) {
            if (!drvPath.isDerivation()) continue;

            jsonRoot[store->printStorePath(drvPath)] =
                store->readDerivation(drvPath).toJSON(*store);
        }
        printJSON(jsonRoot);
    }
};

static auto rCmdShowDerivation = registerCommand2<CmdShowDerivation>({"derivation", "show"});
