#pragma once
///@file

#include <optional>

#ifndef _WIN32
# include <sys/resource.h>
#endif

#include "bsd/util/types.hh"

namespace bsd {

/**
 * If cgroups are active, attempt to calculate the number of CPUs available.
 * If cgroups are unavailable or if cpu.max is set to "max", return 0.
 */
unsigned int getMaxCPU();

// It does not seem possible to dynamically change stack size on Windows.
#ifndef _WIN32
/**
 * Change the stack size.
 */
void setStackSize(size_t stackSize);
#endif

/**
 * Restore the original inherited Ubsd process context (such as signal
 * masks, stack size).

 * See ubsd::startSignalHandlerThread(), ubsd::saveSignalMask().
 */
void restoreProcessContext(bool restoreMounts = true);

/**
 * @return the path of the current executable.
 */
std::optional<Path> getSelfExe();

}
