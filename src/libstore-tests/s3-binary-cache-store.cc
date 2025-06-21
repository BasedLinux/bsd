#include "bsd/store/s3-binary-cache-store.hh"

#if NIX_WITH_S3_SUPPORT

#  include <gtest/gtest.h>

namespace bsd {

TEST(S3BinaryCacheStore, constructConfig)
{
    S3BinaryCacheStoreConfig config{"s3", "foobar", {}};

    EXPECT_EQ(config.bucketName, "foobar");
}

} // namespace bsd

#endif
