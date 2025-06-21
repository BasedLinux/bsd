#pragma once
///@file

#include "bsd/store/store-api.hh"

namespace bsd {

void printGraphML(ref<Store> store, StorePathSet && roots);

}
