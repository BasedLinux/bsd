#include "bsd/store/pathlocks.hh"
#include "bsd/util/util.hh"
#include "bsd/util/sync.hh"
#include "bsd/util/signals.hh"

#include <cerrno>
#include <cstdlib>


namespace bsd {

PathLocks::PathLocks()
    : deletePaths(false)
{
}


PathLocks::PathLocks(const PathSet & paths, const std::string & waitMsg)
    : deletePaths(false)
{
    lockPaths(paths, waitMsg);
}


PathLocks::~PathLocks()
{
    try {
        unlock();
    } catch (...) {
        ignoreExceptionInDestructor();
    }
}


void PathLocks::setDeletion(bool deletePaths)
{
    this->deletePaths = deletePaths;
}


}
