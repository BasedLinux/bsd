#include "bsd/util/processes.hh"
#include "bsd/cmd/command.hh"
#include "bsd/main/common-args.hh"
#include "bsd/store/store-api.hh"
#include "bsd/store/filetransfer.hh"
#include "bsd/expr/eval.hh"
#include "bsd/expr/eval-settings.hh"
#include "bsd/expr/attr-path.hh"
#include "bsd/store/names.hh"
#include "bsd/util/executable-path.hh"
#include "self-exe.hh"

using namespace bsd;

struct CmdUpgradeBsd : MixDryRun, StoreCommand
{
    std::filesystem::path profileDir;

    CmdUpgradeBsd()
    {
        addFlag({
            .longName = "profile",
            .shortName = 'p',
            .description = "The path to the Bsd profile to upgrade.",
            .labels = {"profile-dir"},
            .handler = {&profileDir},
        });

        addFlag({
            .longName = "bsd-store-paths-url",
            .description = "The URL of the file that contains the store paths of the latest Bsd release.",
            .labels = {"url"},
            .handler = {&(std::string&) settings.upgradeBsdStorePathUrl},
        });
    }

    /**
     * This command is stable before the others
     */
    std::optional<ExperimentalFeature> experimentalFeature() override
    {
        return std::nullopt;
    }

    std::string description() override
    {
        return "upgrade Bsd to the latest stable version";
    }

    std::string doc() override
    {
        return
          #include "upgrade-bsd.md"
          ;
    }

    Category category() override { return catBsdInstallation; }

    void run(ref<Store> store) override
    {
        evalSettings.pureEval = true;

        if (profileDir == "")
            profileDir = getProfileDir(store);

        printInfo("upgrading Bsd in profile %s", profileDir);

        auto storePath = getLatestBsd(store);

        auto version = DrvName(storePath.name()).version;

        if (dryRun) {
            logger->stop();
            warn("would upgrade to version %s", version);
            return;
        }

        {
            Activity act(*logger, lvlInfo, actUnknown, fmt("downloading '%s'...", store->printStorePath(storePath)));
            store->ensurePath(storePath);
        }

        {
            Activity act(*logger, lvlInfo, actUnknown, fmt("verifying that '%s' works...", store->printStorePath(storePath)));
            auto program = store->printStorePath(storePath) + "/bin/bsd-env";
            auto s = runProgram(program, false, {"--version"});
            if (s.find("Bsd") == std::string::npos)
                throw Error("could not verify that '%s' works", program);
        }

        logger->stop();

        {
            Activity act(*logger, lvlInfo, actUnknown,
                fmt("installing '%s' into profile %s...", store->printStorePath(storePath), profileDir));

            // FIXME: don't call an external process.
            runProgram(getBsdBin("bsd-env").string(), false,
                {"--profile", profileDir.string(), "-i", store->printStorePath(storePath), "--no-sandbox"});
        }

        printInfo(ANSI_GREEN "upgrade to version %s done" ANSI_NORMAL, version);
    }

    /* Return the profile in which Bsd is installed. */
    std::filesystem::path getProfileDir(ref<Store> store)
    {
        auto whereOpt = ExecutablePath::load().findName(OS_STR("bsd-env"));
        if (!whereOpt)
            throw Error("couldn't figure out how Bsd is installed, so I can't upgrade it");
        const auto & where = whereOpt->parent_path();

        printInfo("found Bsd in %s", where);

        if (hasPrefix(where.string(), "/run/current-system"))
            throw Error("Bsd on BasedLinux must be upgraded via 'bsdos-rebuild'");

        auto profileDir = where.parent_path();

        // Resolve profile to /bsd/var/bsd/profiles/<name> link.
        while (canonPath(profileDir.string()).find("/profiles/") == std::string::npos && std::filesystem::is_symlink(profileDir))
            profileDir = readLink(profileDir.string());

        printInfo("found profile %s", profileDir);

        Path userEnv = canonPath(profileDir.string(), true);

        if (std::filesystem::exists(profileDir / "manifest.json"))
            throw Error("directory %s is managed by 'bsd profile' and currently cannot be upgraded by 'bsd upgrade-bsd'", profileDir);

        if (!std::filesystem::exists(profileDir / "manifest.nix"))
            throw Error("directory %s does not appear to be part of a Bsd profile", profileDir);

        if (!store->isValidPath(store->parseStorePath(userEnv)))
            throw Error("directory '%s' is not in the Bsd store", userEnv);

        return profileDir;
    }

    /* Return the store path of the latest stable Bsd. */
    StorePath getLatestBsd(ref<Store> store)
    {
        Activity act(*logger, lvlInfo, actUnknown, "querying latest Bsd version");

        // FIXME: use basedlinux.org?
        auto req = FileTransferRequest((std::string&) settings.upgradeBsdStorePathUrl);
        auto res = getFileTransfer()->download(req);

        auto state = std::make_unique<EvalState>(LookupPath{}, store, fetchSettings, evalSettings);
        auto v = state->allocValue();
        state->eval(state->parseExprFromString(res.data, state->rootPath(CanonPath("/no-such-path"))), *v);
        Bindings & bindings(*state->allocBindings(0));
        auto v2 = findAlongAttrPath(*state, settings.thisSystem, bindings, *v).first;

        return store->parseStorePath(state->forceString(*v2, noPos, "while evaluating the path tho latest bsd version"));
    }
};

static auto rCmdUpgradeBsd = registerCommand<CmdUpgradeBsd>("upgrade-bsd");
