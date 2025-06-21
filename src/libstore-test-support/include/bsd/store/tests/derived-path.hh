#pragma once
///@file

#include <rapidcheck/gen/Arbitrary.h>

#include "bsd/store/derived-path.hh"

#include "bsd/store/tests/path.hh"
#include "bsd/store/tests/outputs-spec.hh"

namespace rc {
using namespace bsd;

template<>
struct Arbitrary<SingleDerivedPath::Opaque> {
    static Gen<SingleDerivedPath::Opaque> arbitrary();
};

template<>
struct Arbitrary<SingleDerivedPath::Built> {
    static Gen<SingleDerivedPath::Built> arbitrary();
};

template<>
struct Arbitrary<SingleDerivedPath> {
    static Gen<SingleDerivedPath> arbitrary();
};

template<>
struct Arbitrary<DerivedPath::Built> {
    static Gen<DerivedPath::Built> arbitrary();
};

template<>
struct Arbitrary<DerivedPath> {
    static Gen<DerivedPath> arbitrary();
};

}
