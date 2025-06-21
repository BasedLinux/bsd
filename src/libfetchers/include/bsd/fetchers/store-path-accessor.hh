#pragma once

#include "bsd/util/source-path.hh"

namespace bsd {

class StorePath;
class Store;

ref<SourceAccessor> makeStorePathAccessor(ref<Store> store, const StorePath & storePath);

SourcePath getUnfilteredRootPath(CanonPath path);

}
