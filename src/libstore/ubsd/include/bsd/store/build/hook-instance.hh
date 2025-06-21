#pragma once
///@file

#include "bsd/util/logging.hh"
#include "bsd/util/serialise.hh"
#include "bsd/util/processes.hh"

namespace bsd {

struct HookInstance
{
    /**
     * Pipes for talking to the build hook.
     */
    Pipe toHook;

    /**
     * Pipe for the hook's standard output/error.
     */
    Pipe fromHook;

    /**
     * Pipe for the builder's standard output/error.
     */
    Pipe builderOut;

    /**
     * The process ID of the hook.
     */
    Pid pid;

    FdSink sink;

    std::map<ActivityId, Activity> activities;

    HookInstance();

    ~HookInstance();
};

}
