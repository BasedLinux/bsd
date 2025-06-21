#include "bsd/cmd/command.hh"
#include "bsd/main/common-args.hh"
#include "bsd/main/shared.hh"
#include "bsd/store/store-api.hh"
#include "bsd/store/store-cast.hh"
#include "bsd/store/gc-store.hh"

using namespace bsd;

struct CmdStoreGC : StoreCommand, MixDryRun
{
    GCOptions options;

    CmdStoreGC()
    {
        addFlag({
            .longName = "max",
            .description = "Stop after freeing *n* bytes of disk space.",
            .labels = {"n"},
            .handler = {&options.maxFreed},
        });
    }

    std::string description() override
    {
        return "perform garbage collection on a Bsd store";
    }

    std::string doc() override
    {
        return
          #include "store-gc.md"
          ;
    }

    void run(ref<Store> store) override
    {
        auto & gcStore = require<GcStore>(*store);

        options.action = dryRun ? GCOptions::gcReturnDead : GCOptions::gcDeleteDead;
        GCResults results;
        PrintFreed freed(options.action == GCOptions::gcDeleteDead, results);
        gcStore.collectGarbage(options, results);
    }
};

static auto rCmdStoreGC = registerCommand2<CmdStoreGC>({"store", "gc"});
