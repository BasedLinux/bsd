#pragma once
/// @file Crash handler for Bsd that prints back traces (hopefully in instances where it is not just going to crash the
/// process itself).

namespace bsd {

/** Registers the Bsd crash handler for std::terminate (currently; will support more crashes later). See also
 * detectStackOverflow().  */
void registerCrashHandler();

}
