#include "bsd/cmd/command.hh"
#include "bsd/main/common-args.hh"
#include "bsd/main/shared.hh"
#include "bsd/store/store-api.hh"
#include "bsd/store/store-cast.hh"
#include "bsd/store/gc-store.hh"

using namespace bsd;

struct CmdStoreDelete : StorePathsCommand
{
    GCOptions options { .action = GCOptions::gcDeleteSpecific };

    CmdStoreDelete()
    {
        addFlag({
            .longName = "ignore-liveness",
            .description = "Do not check whether the paths are reachable from a root.",
            .handler = {&options.ignoreLiveness, true},
        });
    }

    std::string description() override
    {
        return "delete paths from the Bsd store";
    }

    std::string doc() override
    {
        return
          #include "store-delete.md"
          ;
    }

    void run(ref<Store> store, StorePaths && storePaths) override
    {
        auto & gcStore = require<GcStore>(*store);

        for (auto & path : storePaths)
            options.pathsToDelete.insert(path);

        GCResults results;
        PrintFreed freed(true, results);
        gcStore.collectGarbage(options, results);
    }
};

static auto rCmdStoreDelete = registerCommand2<CmdStoreDelete>({"store", "delete"});
