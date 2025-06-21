#pragma once
///@file

#include "bsd/util/serialise.hh"
#include "bsd/store/store-api.hh"

namespace bsd::daemon {

enum RecursiveFlag : bool { NotRecursive = false, Recursive = true };

void processConnection(
    ref<Store> store,
    FdSource && from,
    FdSink && to,
    TrustedFlag trusted,
    RecursiveFlag recursive);

}
