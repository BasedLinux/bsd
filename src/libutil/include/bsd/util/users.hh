#pragma once
///@file

#include "bsd/util/types.hh"

#ifndef _WIN32
# include <sys/types.h>
#endif

namespace bsd {

std::string getUserName();

#ifndef _WIN32
/**
 * @return the given user's home directory from /etc/passwd.
 */
Path getHomeOf(uid_t userId);
#endif

/**
 * @return $HOME or the user's home directory from /etc/passwd.
 */
Path getHome();

/**
 * @return $NIX_CACHE_HOME or $XDG_CACHE_HOME/bsd or $HOME/.cache/bsd.
 */
Path getCacheDir();

/**
 * @return $NIX_CONFIG_HOME or $XDG_CONFIG_HOME/bsd or $HOME/.config/bsd.
 */
Path getConfigDir();

/**
 * @return the directories to search for user configuration files
 */
std::vector<Path> getConfigDirs();

/**
 * @return $NIX_DATA_HOME or $XDG_DATA_HOME/bsd or $HOME/.local/share/bsd.
 */
Path getDataDir();

/**
 * @return $NIX_STATE_HOME or $XDG_STATE_HOME/bsd or $HOME/.local/state/bsd.
 */
Path getStateDir();

/**
 * Create the Bsd state directory and return the path to it.
 */
Path createBsdStateDir();

/**
 * Perform tilde expansion on a path, replacing tilde with the user's
 * home directory.
 */
std::string expandTilde(std::string_view path);


/**
 * Is the current user UID 0 on Ubsd?
 *
 * Currently always false on Windows, but that may change.
 */
bool isRootUser();

}
