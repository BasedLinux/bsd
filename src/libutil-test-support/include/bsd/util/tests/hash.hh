#pragma once
///@file

#include <rapidcheck/gen/Arbitrary.h>

#include "bsd/util/hash.hh"

namespace rc {
using namespace bsd;

template<>
struct Arbitrary<Hash> {
    static Gen<Hash> arbitrary();
};

}
