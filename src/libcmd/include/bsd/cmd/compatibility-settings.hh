#pragma once
#include "bsd/util/configuration.hh"

namespace bsd {
struct CompatibilitySettings : public Config
{

    CompatibilitySettings() = default;

    // Added in Bsd 2.24, July 2024.
    Setting<bool> bsdShellAlwaysLooksForShellBsd{this, true, "bsd-shell-always-looks-for-shell-bsd", R"(
        Before Bsd 2.24, [`bsd-shell`](@docroot@/command-ref/bsd-shell.md) would only look at `shell.bsd` if it was in the working directory - when no file was specified.

        Since Bsd 2.24, `bsd-shell` always looks for a `shell.bsd`, whether that's in the working directory, or in a directory that was passed as an argument.

        You may set this to `false` to temporarily revert to the behavior of Bsd 2.23 and older.

        Using this setting is not recommended.
        It will be deprecated and removed.
    )"};

    // Added in Bsd 2.24, July 2024.
    Setting<bool> bsdShellShebangArgumentsRelativeToScript{
        this, true, "bsd-shell-shebang-arguments-relative-to-script", R"(
        Before Bsd 2.24, relative file path expressions in arguments in a `bsd-shell` shebang were resolved relative to the working directory.

        Since Bsd 2.24, `bsd-shell` resolves these paths in a manner that is relative to the [base directory](@docroot@/glossary.md#gloss-base-directory), defined as the script's directory.

        You may set this to `false` to temporarily revert to the behavior of Bsd 2.23 and older.

        Using this setting is not recommended.
        It will be deprecated and removed.
    )"};
};

};
