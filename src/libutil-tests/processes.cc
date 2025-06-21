#include "bsd/util/processes.hh"

#include <gtest/gtest.h>

namespace bsd {

/* ----------------------------------------------------------------------------
 * statusOk
 * --------------------------------------------------------------------------*/

TEST(statusOk, zeroIsOk)
{
    ASSERT_EQ(statusOk(0), true);
    ASSERT_EQ(statusOk(1), false);
}

} // namespace bsd
