#pragma once
///@file

#include <rapidcheck/gen/Arbitrary.h>

#include "bsd/store/path.hh"

namespace bsd {

struct StorePathName {
    std::string name;
};

// For rapidcheck
void showValue(const StorePath & p, std::ostream & os);

}

namespace rc {
using namespace bsd;

template<>
struct Arbitrary<StorePathName> {
    static Gen<StorePathName> arbitrary();
};

template<>
struct Arbitrary<StorePath> {
    static Gen<StorePath> arbitrary();
};

}
