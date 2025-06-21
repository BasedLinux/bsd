// FIXME: rename to 'bsd plan add' or 'bsd derivation add'?

#include "bsd/cmd/command.hh"
#include "bsd/main/common-args.hh"
#include "bsd/store/store-api.hh"
#include "bsd/util/archive.hh"
#include "bsd/store/derivations.hh"
#include <nlohmann/json.hpp>

using namespace bsd;
using json = nlohmann::json;

struct CmdAddDerivation : MixDryRun, StoreCommand
{
    std::string description() override
    {
        return "Add a store derivation";
    }

    std::string doc() override
    {
        return
          #include "derivation-add.md"
          ;
    }

    Category category() override { return catUtility; }

    void run(ref<Store> store) override
    {
        auto json = nlohmann::json::parse(drainFD(STDIN_FILENO));

        auto drv = Derivation::fromJSON(*store, json);

        auto drvPath = writeDerivation(*store, drv, NoRepair, /* read only */ dryRun);

        drv.checkInvariants(*store, drvPath);

        writeDerivation(*store, drv, NoRepair, dryRun);

        logger->cout("%s", store->printStorePath(drvPath));
    }
};

static auto rCmdAddDerivation = registerCommand2<CmdAddDerivation>({"derivation", "add"});
