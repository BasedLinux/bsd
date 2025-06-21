#include "bsd/store/build-result.hh"

namespace bsd {

bool BuildResult::operator==(const BuildResult &) const noexcept = default;
std::strong_ordering BuildResult::operator<=>(const BuildResult &) const noexcept = default;

}
