#pragma once
/**
 * @file
 *
 * Implementation of some inline definitions for Ubsd signals, and also
 * some extra Ubsd-only interfaces.
 *
 * (The only reason everything about signals isn't Ubsd-only is some
 * no-op definitions are provided on Windows to avoid excess CPP in
 * downstream code.)
 */

#include "bsd/util/types.hh"
#include "bsd/util/error.hh"
#include "bsd/util/logging.hh"
#include "bsd/util/ansicolor.hh"
#include "bsd/util/signals.hh"

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>

#include <boost/lexical_cast.hpp>

#include <atomic>
#include <functional>
#include <map>
#include <sstream>
#include <optional>

namespace bsd {

/* User interruption. */

namespace ubsd {

extern std::atomic<bool> _isInterrupted;

extern thread_local std::function<bool()> interruptCheck;

void _interrupted();

/**
 * Sets the signal mask. Like saveSignalMask() but for a signal set that doesn't
 * necessarily match the current thread's mask.
 * See saveSignalMask() to set the saved mask to the current mask.
 */
void setChildSignalMask(sigset_t *sigs);

/**
 * Start a thread that handles various signals. Also block those signals
 * on the current thread (and thus any threads created by it).
 * Saves the signal mask before changing the mask to block those signals.
 * See saveSignalMask().
 */
void startSignalHandlerThread();

/**
 * Saves the signal mask, which is the signal mask that bsd will restore
 * before creating child processes.
 * See setChildSignalMask() to set an arbitrary signal mask instead of the
 * current mask.
 */
void saveSignalMask();

/**
 * To use in a process that already called `startSignalHandlerThread()`
 * or `saveSignalMask()` first.
 */
void restoreSignals();

void triggerInterrupt();

}

static inline void setInterrupted(bool isInterrupted)
{
    ubsd::_isInterrupted = isInterrupted;
}

static inline bool getInterrupted()
{
    return ubsd::_isInterrupted;
}

static inline bool isInterrupted()
{
    using namespace ubsd;
    return _isInterrupted || (interruptCheck && interruptCheck());
}

/**
 * Throw `Interrupted` exception if the process has been interrupted.
 *
 * Call this in long-running loops and between slow operations to terminate
 * them as needed.
 */
inline void checkInterrupt()
{
    if (isInterrupted())
        ubsd::_interrupted();
}

/**
 * A RAII class that causes the current thread to receive SIGUSR1 when
 * the signal handler thread receives SIGINT. That is, this allows
 * SIGINT to be multiplexed to multiple threads.
 */
struct ReceiveInterrupts
{
    pthread_t target;
    std::unique_ptr<InterruptCallback> callback;

    ReceiveInterrupts()
        : target(pthread_self())
        , callback(createInterruptCallback([&]() { pthread_kill(target, SIGUSR1); }))
    { }
};


}
