#include "bsd/store/store-dir-config.hh"
#include "bsd/util/util.hh"
#include "bsd/store/globals.hh"

namespace bsd {

StoreDirConfig::StoreDirConfig(const Params & params)
    : StoreDirConfigBase(params)
    , MixStoreDirMethods{storeDir_}
{
}

}
