#include "bsd/fetchers/store-path-accessor.hh"
#include "bsd/store/store-api.hh"

namespace bsd {

ref<SourceAccessor> makeStorePathAccessor(ref<Store> store, const StorePath & storePath)
{
    return projectSubdirSourceAccessor(store->getFSAccessor(), storePath.to_string());
}

}
