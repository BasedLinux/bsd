#include <rapidcheck.h>

#include "bsd/store/tests/path.hh"
#include "bsd/expr/tests/value/context.hh"

namespace rc {
using namespace bsd;

Gen<BsdStringContextElem::DrvDeep> Arbitrary<BsdStringContextElem::DrvDeep>::arbitrary()
{
    return gen::map(gen::arbitrary<StorePath>(), [](StorePath drvPath) {
        return BsdStringContextElem::DrvDeep{
            .drvPath = drvPath,
        };
    });
}

Gen<BsdStringContextElem> Arbitrary<BsdStringContextElem>::arbitrary()
{
    return gen::mapcat(
        gen::inRange<uint8_t>(0, std::variant_size_v<BsdStringContextElem::Raw>),
        [](uint8_t n) -> Gen<BsdStringContextElem> {
            switch (n) {
            case 0:
                return gen::map(
                    gen::arbitrary<BsdStringContextElem::Opaque>(), [](BsdStringContextElem a) { return a; });
            case 1:
                return gen::map(
                    gen::arbitrary<BsdStringContextElem::DrvDeep>(), [](BsdStringContextElem a) { return a; });
            case 2:
                return gen::map(
                    gen::arbitrary<BsdStringContextElem::Built>(), [](BsdStringContextElem a) { return a; });
            default:
                assert(false);
            }
        });
}

}
