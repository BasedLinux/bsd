#include "bsd/cmd/command.hh"
#include "bsd/main/common-args.hh"
#include "bsd/store/store-api.hh"
#include "bsd/util/archive.hh"
#include "bsd/util/git.hh"
#include "bsd/util/posix-source-accessor.hh"
#include "bsd/cmd/misc-store-flags.hh"

using namespace bsd;

struct CmdAddToStore : MixDryRun, StoreCommand
{
    Path path;
    std::optional<std::string> namePart;
    ContentAddressMethod caMethod = ContentAddressMethod::Raw::BsdArchive;
    HashAlgorithm hashAlgo = HashAlgorithm::SHA256;

    CmdAddToStore()
    {
        // FIXME: completion
        expectArg("path", &path);

        addFlag({
            .longName = "name",
            .shortName = 'n',
            .description = "Override the name component of the store path. It defaults to the base name of *path*.",
            .labels = {"name"},
            .handler = {&namePart},
        });

        addFlag(flag::contentAddressMethod(&caMethod));

        addFlag(flag::hashAlgo(&hashAlgo));
    }

    void run(ref<Store> store) override
    {
        if (!namePart) namePart = baseNameOf(path);

        auto sourcePath = PosixSourceAccessor::createAtRoot(makeParentCanonical(path));

        auto storePath = dryRun
            ? store->computeStorePath(
                *namePart, sourcePath, caMethod, hashAlgo, {}).first
            : store->addToStoreSlow(
                *namePart, sourcePath, caMethod, hashAlgo, {}).path;

        logger->cout("%s", store->printStorePath(storePath));
    }
};

struct CmdAdd : CmdAddToStore
{
    std::string description() override
    {
        return "Add a file or directory to the Bsd store";
    }

    std::string doc() override
    {
        return
          #include "add.md"
          ;
    }
};

struct CmdAddFile : CmdAddToStore
{
    CmdAddFile()
    {
        caMethod = ContentAddressMethod::Raw::Flat;
    }

    std::string description() override
    {
        return "Deprecated. Use [`bsd store add --mode flat`](@docroot@/command-ref/new-cli/bsd3-store-add.md) instead.";
    }
};

struct CmdAddPath : CmdAddToStore
{
    std::string description() override
    {
        return "Deprecated alias to [`bsd store add`](@docroot@/command-ref/new-cli/bsd3-store-add.md).";
    }
};

static auto rCmdAddFile = registerCommand2<CmdAddFile>({"store", "add-file"});
static auto rCmdAddPath = registerCommand2<CmdAddPath>({"store", "add-path"});
static auto rCmdAdd = registerCommand2<CmdAdd>({"store", "add"});
