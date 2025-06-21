#pragma once
///@file

#include "bsd/util/types.hh"
#include "bsd/util/file-descriptor.hh"

#ifdef _WIN32
#  include <winsock2.h>
#endif
#include <unistd.h>

#include <filesystem>

namespace bsd {

/**
 * Create a Ubsd domain socket.
 */
AutoCloseFD createUbsdDomainSocket();

/**
 * Create a Ubsd domain socket in listen mode.
 */
AutoCloseFD createUbsdDomainSocket(const Path & path, mode_t mode);

/**
 * Often we want to use `Descriptor`, but Windows makes a slightly
 * stronger file descriptor vs socket distinction, at least at the level
 * of C types.
 */
using Socket =
#ifdef _WIN32
    SOCKET
#else
    int
#endif
    ;

#ifdef _WIN32
/**
 * Windows gives this a different name
 */
#  define SHUT_WR SD_SEND
#  define SHUT_RDWR SD_BOTH
#endif

/**
 * Convert a `Socket` to a `Descriptor`
 *
 * This is a no-op except on Windows.
 */
static inline Socket toSocket(Descriptor fd)
{
#ifdef _WIN32
    return reinterpret_cast<Socket>(fd);
#else
    return fd;
#endif
}

/**
 * Convert a `Socket` to a `Descriptor`
 *
 * This is a no-op except on Windows.
 */
static inline Descriptor fromSocket(Socket fd)
{
#ifdef _WIN32
    return reinterpret_cast<Descriptor>(fd);
#else
    return fd;
#endif
}

/**
 * Bind a Ubsd domain socket to a path.
 */
void bind(Socket fd, const std::string & path);

/**
 * Connect to a Ubsd domain socket.
 */
void connect(Socket fd, const std::filesystem::path & path);

/**
 * Connect to a Ubsd domain socket.
 */
AutoCloseFD connect(const std::filesystem::path & path);

}
