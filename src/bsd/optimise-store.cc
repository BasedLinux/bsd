#include "bsd/cmd/command.hh"
#include "bsd/main/shared.hh"
#include "bsd/store/store-api.hh"

#include <atomic>

using namespace bsd;

struct CmdOptimiseStore : StoreCommand
{
    std::string description() override
    {
        return "replace identical files in the store by hard links";
    }

    std::string doc() override
    {
        return
          #include "optimise-store.md"
          ;
    }

    void run(ref<Store> store) override
    {
        store->optimiseStore();
    }
};

static auto rCmdOptimiseStore = registerCommand2<CmdOptimiseStore>({"store", "optimise"});
