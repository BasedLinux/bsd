#pragma once
///@file

#include <rapidcheck/gen/Arbitrary.h>

#include "bsd/store/outputs-spec.hh"

#include "bsd/store/tests/path.hh"

namespace rc {
using namespace bsd;

template<>
struct Arbitrary<OutputsSpec> {
    static Gen<OutputsSpec> arbitrary();
};

}
