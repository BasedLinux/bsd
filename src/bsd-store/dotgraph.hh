#pragma once
///@file

#include "bsd/store/store-api.hh"

namespace bsd {

void printDotGraph(ref<Store> store, StorePathSet && roots);

}
