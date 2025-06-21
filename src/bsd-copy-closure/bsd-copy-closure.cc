#include "bsd/main/shared.hh"
#include "bsd/store/realisation.hh"
#include "bsd/store/store-open.hh"
#include "bsd/cmd/legacy.hh"
#include "man-pages.hh"

using namespace bsd;

static int main_bsd_copy_closure(int argc, char ** argv)
{
    {
        auto gzip = false;
        auto toMode = true;
        auto includeOutputs = false;
        auto dryRun = false;
        auto useSubstitutes = NoSubstitute;
        std::string sshHost;
        PathSet storePaths;

        parseCmdLine(argc, argv, [&](Strings::iterator & arg, const Strings::iterator & end) {
            if (*arg == "--help")
                showManPage("bsd-copy-closure");
            else if (*arg == "--version")
                printVersion("bsd-copy-closure");
            else if (*arg == "--gzip" || *arg == "--bzip2" || *arg == "--xz") {
                if (*arg != "--gzip")
                    warn("'%1%' is not implemented, falling back to gzip", *arg);
                gzip = true;
            } else if (*arg == "--from")
                toMode = false;
            else if (*arg == "--to")
                toMode = true;
            else if (*arg == "--include-outputs")
                includeOutputs = true;
            else if (*arg == "--show-progress")
                printMsg(lvlError, "Warning: '--show-progress' is not implemented");
            else if (*arg == "--dry-run")
                dryRun = true;
            else if (*arg == "--use-substitutes" || *arg == "-s")
                useSubstitutes = Substitute;
            else if (sshHost.empty())
                sshHost = *arg;
            else
                storePaths.insert(*arg);
            return true;
        });

        if (sshHost.empty())
            throw UsageError("no host name specified");

        auto remoteUri = "ssh://" + sshHost + (gzip ? "?compress=true" : "");
        auto to = toMode ? openStore(remoteUri) : openStore();
        auto from = toMode ? openStore() : openStore(remoteUri);

        RealisedPath::Set storePaths2;
        for (auto & path : storePaths)
            storePaths2.insert(from->followLinksToStorePath(path));

        copyClosure(*from, *to, storePaths2, NoRepair, NoCheckSigs, useSubstitutes);

        return 0;
    }
}

static RegisterLegacyCommand r_bsd_copy_closure("bsd-copy-closure", main_bsd_copy_closure);
