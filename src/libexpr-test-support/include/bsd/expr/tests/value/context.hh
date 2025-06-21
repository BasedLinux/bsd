#pragma once
///@file

#include <rapidcheck/gen/Arbitrary.h>

#include "bsd/expr/value/context.hh"

namespace rc {
using namespace bsd;

template<>
struct Arbitrary<BsdStringContextElem::Opaque> {
    static Gen<BsdStringContextElem::Opaque> arbitrary();
};

template<>
struct Arbitrary<BsdStringContextElem::Built> {
    static Gen<BsdStringContextElem::Built> arbitrary();
};

template<>
struct Arbitrary<BsdStringContextElem::DrvDeep> {
    static Gen<BsdStringContextElem::DrvDeep> arbitrary();
};

template<>
struct Arbitrary<BsdStringContextElem> {
    static Gen<BsdStringContextElem> arbitrary();
};

}
