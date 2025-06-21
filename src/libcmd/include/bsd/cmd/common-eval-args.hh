#pragma once
///@file

#include "bsd/util/args.hh"
#include "bsd/util/canon-path.hh"
#include "bsd/main/common-args.hh"
#include "bsd/expr/search-path.hh"

#include <filesystem>

namespace bsd {

class Store;

namespace fetchers { struct Settings; }

class EvalState;
struct EvalSettings;
struct CompatibilitySettings;
class Bindings;
struct SourcePath;

namespace flake { struct Settings; }

/**
 * @todo Get rid of global settings variables
 */
extern fetchers::Settings fetchSettings;

/**
 * @todo Get rid of global settings variables
 */
extern EvalSettings evalSettings;

/**
 * @todo Get rid of global settings variables
 */
extern flake::Settings flakeSettings;

/**
 * Settings that control behaviors that have changed since Bsd 2.3.
 */
extern CompatibilitySettings compatibilitySettings;

struct MixEvalArgs : virtual Args, virtual MixRepair
{
    static constexpr auto category = "Common evaluation options";

    MixEvalArgs();

    Bindings * getAutoArgs(EvalState & state);

    LookupPath lookupPath;

    std::optional<std::string> evalStoreUrl;

private:
    struct AutoArgExpr { std::string expr; };
    struct AutoArgString { std::string s; };
    struct AutoArgFile { std::filesystem::path path; };
    struct AutoArgStdin { };

    using AutoArg = std::variant<AutoArgExpr, AutoArgString, AutoArgFile, AutoArgStdin>;

    std::map<std::string, AutoArg> autoArgs;
};

/**
 * @param baseDir Optional [base directory](https://basedlinux.org/manual/bsd/unstable/glossary#gloss-base-directory)
 */
SourcePath lookupFileArg(EvalState & state, std::string_view s, const Path * baseDir = nullptr);

}
