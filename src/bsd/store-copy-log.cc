#include "bsd/cmd/command.hh"
#include "bsd/main/shared.hh"
#include "bsd/store/store-api.hh"
#include "bsd/store/store-cast.hh"
#include "bsd/store/log-store.hh"
#include "bsd/util/sync.hh"
#include "bsd/util/thread-pool.hh"

#include <atomic>

using namespace bsd;

struct CmdCopyLog : virtual CopyCommand, virtual InstallablesCommand
{
    std::string description() override
    {
        return "copy build logs between Bsd stores";
    }

    std::string doc() override
    {
        return
          #include "store-copy-log.md"
          ;
    }

    void run(ref<Store> srcStore, Installables && installables) override
    {
        auto & srcLogStore = require<LogStore>(*srcStore);

        auto dstStore = getDstStore();
        auto & dstLogStore = require<LogStore>(*dstStore);

        for (auto & drvPath : Installable::toDerivations(getEvalStore(), installables, true)) {
            if (auto log = srcLogStore.getBuildLog(drvPath))
                dstLogStore.addBuildLog(drvPath, *log);
            else
                throw Error("build log for '%s' is not available", srcStore->printStorePath(drvPath));
        }
    }
};

static auto rCmdCopyLog = registerCommand2<CmdCopyLog>({"store", "copy-log"});
