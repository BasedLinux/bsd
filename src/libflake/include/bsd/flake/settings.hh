#pragma once
///@file

#include "bsd/util/configuration.hh"

#include <sys/types.h>

namespace bsd {
// Forward declarations
struct EvalSettings;

} // namespace bsd

namespace bsd::flake {

struct Settings : public Config
{
    Settings();

    void configureEvalSettings(bsd::EvalSettings & evalSettings) const;

    Setting<bool> useRegistries{
        this,
        true,
        "use-registries",
        "Whether to use flake registries to resolve flake references.",
        {},
        true,
        Xp::Flakes};

    Setting<bool> acceptFlakeConfig{
        this,
        false,
        "accept-flake-config",
        "Whether to accept Bsd configuration settings from a flake without prompting.",
        {},
        true,
        Xp::Flakes};

    Setting<std::string> commitLockFileSummary{
        this,
        "",
        "commit-lock-file-summary",
        R"(
          The commit summary to use when committing changed flake lock files. If
          empty, the summary is generated based on the action performed.
        )",
        {"commit-lockfile-summary"},
        true,
        Xp::Flakes};
};

}
