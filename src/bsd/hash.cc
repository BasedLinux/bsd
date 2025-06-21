#include "bsd/cmd/command.hh"
#include "bsd/util/hash.hh"
#include "bsd/store/content-address.hh"
#include "bsd/cmd/legacy.hh"
#include "bsd/main/shared.hh"
#include "bsd/util/references.hh"
#include "bsd/util/archive.hh"
#include "bsd/util/git.hh"
#include "bsd/util/posix-source-accessor.hh"
#include "bsd/cmd/misc-store-flags.hh"
#include "man-pages.hh"

using namespace bsd;

/**
 * Base for `bsd hash path`, `bsd hash file` (deprecated), and `bsd-hash` (legacy).
 *
 * Deprecation Issue: https://github.com/BasedLinux/bsd/issues/8876
 */
struct CmdHashBase : Command
{
    FileIngestionMethod mode;
    HashFormat hashFormat = HashFormat::SRI;
    bool truncate = false;
    HashAlgorithm hashAlgo = HashAlgorithm::SHA256;
    std::vector<std::string> paths;
    std::optional<std::string> modulus;

    explicit CmdHashBase(FileIngestionMethod mode) : mode(mode)
    {
        expectArgs({
            .label = "paths",
            .handler = {&paths},
            .completer = completePath
        });

        // FIXME The following flags should be deprecated, but we don't
        // yet have a mechanism for that.

        addFlag({
            .longName = "sri",
            .description = "Print the hash in SRI format.",
            .handler = {&hashFormat, HashFormat::SRI},
        });

        addFlag({
            .longName = "base64",
            .description = "Print the hash in base-64 format.",
            .handler = {&hashFormat, HashFormat::Base64},
        });

        addFlag({
            .longName = "base32",
            .description = "Print the hash in base-32 (Bsd-specific) format.",
            .handler = {&hashFormat, HashFormat::Bsd32},
        });

        addFlag({
            .longName = "base16",
            .description = "Print the hash in base-16 format.",
            .handler = {&hashFormat, HashFormat::Base16},
        });

        addFlag(flag::hashAlgo("type", &hashAlgo));
    }

    std::string description() override
    {
        switch (mode) {
        case FileIngestionMethod::Flat:
            return "print cryptographic hash of a regular file";
        case FileIngestionMethod::BsdArchive:
            return "print cryptographic hash of the NAR serialisation of a path";
        case FileIngestionMethod::Git:
            return "print cryptographic hash of the Git serialisation of a path";
        default:
            assert(false);
        };
    }

    void run() override
    {
        for (const auto & path : paths) {
            auto makeSink = [&]() -> std::unique_ptr<AbstractHashSink> {
                if (modulus)
                    return std::make_unique<HashModuloSink>(hashAlgo, *modulus);
                else
                    return std::make_unique<HashSink>(hashAlgo);
            };

            auto makeSourcePath = [&]() -> SourcePath {
                return PosixSourceAccessor::createAtRoot(makeParentCanonical(path));
            };

            Hash h { HashAlgorithm::SHA256 }; // throwaway def to appease C++
            switch (mode) {
            case FileIngestionMethod::Flat:
            {
                // While usually we could use the some code as for BsdArchive,
                // the Flat method needs to support FIFOs, such as those
                // produced by bash process substitution, e.g.:
                //     bsd hash --mode flat <(echo hi)
                // Also symlinks semantics are unambiguous in the flat case,
                // so we don't need to go low-level, or reject symlink `path`s.
                auto hashSink = makeSink();
                readFile(path, *hashSink);
                h = hashSink->finish().first;
                break;
            }
            case FileIngestionMethod::BsdArchive:
            {
                auto sourcePath = makeSourcePath();
                auto hashSink = makeSink();
                dumpPath(sourcePath, *hashSink, (FileSerialisationMethod) mode);
                h = hashSink->finish().first;
                break;
            }
            case FileIngestionMethod::Git: {
                auto sourcePath = makeSourcePath();
                std::function<git::DumpHook> hook;
                hook = [&](const SourcePath & path) -> git::TreeEntry {
                    auto hashSink = makeSink();
                    auto mode = dump(path, *hashSink, hook);
                    auto hash = hashSink->finish().first;
                    return {
                        .mode = mode,
                        .hash = hash,
                    };
                };
                h = hook(sourcePath).hash;
                break;
            }
            }

            if (truncate && h.hashSize > 20) h = compressHash(h, 20);
            logger->cout(h.to_string(hashFormat, hashFormat == HashFormat::SRI));
        }
    }
};

/**
 * `bsd hash path`
 */
struct CmdHashPath : CmdHashBase
{
    CmdHashPath()
        : CmdHashBase(FileIngestionMethod::BsdArchive)
    {
        addFlag(flag::hashAlgo("algo", &hashAlgo));
        addFlag(flag::fileIngestionMethod(&mode));
        addFlag(flag::hashFormatWithDefault("format", &hashFormat));
        #if 0
        addFlag({
            .longName = "modulo",
            .description = "Compute the hash modulo the specified string.",
            .labels = {"modulus"},
            .handler = {&modulus},
        });
        #endif
    }
};

/**
 * For deprecated `bsd hash file`
 *
 * Deprecation Issue: https://github.com/BasedLinux/bsd/issues/8876
 */
struct CmdHashFile : CmdHashBase
{
    CmdHashFile()
        : CmdHashBase(FileIngestionMethod::Flat)
    {
    }
};

/**
 * For deprecated `bsd hash to-*`
 */
struct CmdToBase : Command
{
    HashFormat hashFormat;
    std::optional<HashAlgorithm> hashAlgo;
    std::vector<std::string> args;
    bool legacyCli;

    CmdToBase(HashFormat hashFormat, bool legacyCli = false)
        : hashFormat(hashFormat)
        , legacyCli(legacyCli)
    {
        addFlag(flag::hashAlgoOpt("type", &hashAlgo));
        expectArgs("strings", &args);
    }

    std::string description() override
    {
        return fmt("convert a hash to %s representation (deprecated, use `bsd hash convert` instead)",
            hashFormat == HashFormat::Base16 ? "base-16" :
            hashFormat == HashFormat::Bsd32 ? "base-32" :
            hashFormat == HashFormat::Base64 ? "base-64" :
            "SRI");
    }

    void run() override
    {
        if (!legacyCli)
            warn("The old format conversion subcommands of `bsd hash` were deprecated in favor of `bsd hash convert`.");
        for (const auto & s : args)
            logger->cout(Hash::parseAny(s, hashAlgo).to_string(hashFormat, hashFormat == HashFormat::SRI));
    }
};

/**
 * `bsd hash convert`
 */
struct CmdHashConvert : Command
{
    std::optional<HashFormat> from;
    HashFormat to;
    std::optional<HashAlgorithm> algo;
    std::vector<std::string> hashStrings;

    CmdHashConvert(): to(HashFormat::SRI) {
        addFlag(flag::hashFormatOpt("from", &from));
        addFlag(flag::hashFormatWithDefault("to", &to));
        addFlag(flag::hashAlgoOpt(&algo));
        expectArgs({
           .label = "hashes",
           .handler = {&hashStrings},
        });
    }

    std::string description() override
    {
        return "convert between hash formats";
    }

    std::string doc() override
    {
        return
          #include "hash-convert.md"
          ;
    }

    Category category() override { return catUtility; }

    void run() override {
        for (const auto & s : hashStrings) {
            Hash h =
                from == HashFormat::SRI
                ? Hash::parseSRI(s)
                : Hash::parseAny(s, algo);
            if (from
                && from != HashFormat::SRI
                && h.to_string(*from, false) !=
                    (from == HashFormat::Base16 ? toLower(s) : s))
            {
                auto from_as_string = printHashFormat(*from);
                throw BadHash("input hash '%s' does not have the expected format for '--from %s'", s, from_as_string);
            }
            logger->cout(h.to_string(to, to == HashFormat::SRI));
        }
    }
};

struct CmdHash : BsdMultiCommand
{
    CmdHash()
        : BsdMultiCommand(
            "hash",
            {
                {"convert", []() { return make_ref<CmdHashConvert>();}},
                {"path", []() { return make_ref<CmdHashPath>(); }},
                {"file", []() { return make_ref<CmdHashFile>(); }},
                {"to-base16", []() { return make_ref<CmdToBase>(HashFormat::Base16); }},
                {"to-base32", []() { return make_ref<CmdToBase>(HashFormat::Bsd32); }},
                {"to-base64", []() { return make_ref<CmdToBase>(HashFormat::Base64); }},
                {"to-sri", []() { return make_ref<CmdToBase>(HashFormat::SRI); }},
          })
    { }

    std::string description() override
    {
        return "compute and convert cryptographic hashes";
    }

    Category category() override { return catUtility; }
};

static auto rCmdHash = registerCommand<CmdHash>("hash");

/* Legacy bsd-hash command. */
static int compatBsdHash(int argc, char * * argv)
{
    // Wait until `bsd hash convert` is not hidden behind experimental flags anymore.
    // warn("`bsd-hash` has been deprecated in favor of `bsd hash convert`.");

    std::optional<HashAlgorithm> hashAlgo;
    bool flat = false;
    HashFormat hashFormat = HashFormat::Base16;
    bool truncate = false;
    enum { opHash, opTo } op = opHash;
    std::vector<std::string> ss;

    parseCmdLine(argc, argv, [&](Strings::iterator & arg, const Strings::iterator & end) {
        if (*arg == "--help")
            showManPage("bsd-hash");
        else if (*arg == "--version")
            printVersion("bsd-hash");
        else if (*arg == "--flat") flat = true;
        else if (*arg == "--base16") hashFormat = HashFormat::Base16;
        else if (*arg == "--base32") hashFormat = HashFormat::Bsd32;
        else if (*arg == "--base64") hashFormat = HashFormat::Base64;
        else if (*arg == "--sri") hashFormat = HashFormat::SRI;
        else if (*arg == "--truncate") truncate = true;
        else if (*arg == "--type") {
            std::string s = getArg(*arg, arg, end);
            hashAlgo = parseHashAlgo(s);
        }
        else if (*arg == "--to-base16") {
            op = opTo;
            hashFormat = HashFormat::Base16;
        }
        else if (*arg == "--to-base32") {
            op = opTo;
            hashFormat = HashFormat::Bsd32;
        }
        else if (*arg == "--to-base64") {
            op = opTo;
            hashFormat = HashFormat::Base64;
        }
        else if (*arg == "--to-sri") {
            op = opTo;
            hashFormat = HashFormat::SRI;
        }
        else if (*arg != "" && arg->at(0) == '-')
            return false;
        else
            ss.push_back(*arg);
        return true;
    });

    if (op == opHash) {
        CmdHashBase cmd(flat ? FileIngestionMethod::Flat : FileIngestionMethod::BsdArchive);
        if (!hashAlgo.has_value()) hashAlgo = HashAlgorithm::MD5;
        cmd.hashAlgo = hashAlgo.value();
        cmd.hashFormat = hashFormat;
        cmd.truncate = truncate;
        cmd.paths = ss;
        cmd.run();
    }

    else {
        CmdToBase cmd(hashFormat, true);
        cmd.args = ss;
        if (hashAlgo.has_value()) cmd.hashAlgo = hashAlgo;
        cmd.run();
    }

    return 0;
}

static RegisterLegacyCommand r_bsd_hash("bsd-hash", compatBsdHash);
