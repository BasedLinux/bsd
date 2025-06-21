#pragma once
///@file

#include "bsd/util/types.hh"

namespace bsd {

/* User interruption. */

static inline void setInterrupted(bool isInterrupted)
{
    /* Do nothing for now */
}

static inline bool getInterrupted()
{
    return false;
}

inline void setInterruptThrown()
{
    /* Do nothing for now */
}

static inline bool isInterrupted()
{
    /* Do nothing for now */
    return false;
}

inline void checkInterrupt()
{
    /* Do nothing for now */
}

/**
 * Does nothing, unlike Ubsd counterpart, but allows avoiding C++
 */
struct ReceiveInterrupts
{
    /**
     * Explicit destructor avoids dead code warnings.
     */
    ~ReceiveInterrupts() {}
};

}
