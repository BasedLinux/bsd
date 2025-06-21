#pragma once
///@file

#include <filesystem>
#include <string>

namespace bsd {

/**
 * @brief Get path to the bsd manual dir.
 *
 * Bsd relies on the man pages being available at a NIX_MAN_DIR for
 * displaying help messaged for legacy cli.
 *
 * NIX_MAN_DIR is a compile-time parameter, so man pages are unlikely to work
 * for cases when the bsd executable is installed out-of-store or as a static binary.
 *
 */
std::filesystem::path getBsdManDir();

/**
 * Show the manual page for the specified program.
 *
 * @param name Name of the man item.
 */
void showManPage(const std::string & name);

}
