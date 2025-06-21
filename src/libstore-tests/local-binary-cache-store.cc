#include <gtest/gtest.h>

#include "bsd/store/local-binary-cache-store.hh"

namespace bsd {

TEST(LocalBinaryCacheStore, constructConfig)
{
    LocalBinaryCacheStoreConfig config{"local", "/foo/bar/baz", {}};

    EXPECT_EQ(config.binaryCacheDir, "/foo/bar/baz");
}

} // namespace bsd
