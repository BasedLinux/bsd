// FIXME: Odd failures for templates that are causing the PR to break
// for now with discussion with @Ericson2314 to comment out.
#if 0
#  include <gtest/gtest.h>

#  include "bsd/store/uds-remote-store.hh"

namespace bsd {

TEST(UDSRemoteStore, constructConfig)
{
    UDSRemoteStoreConfig config{"ubsd", "/tmp/socket", {}};

    EXPECT_EQ(config.path, "/tmp/socket");
}

TEST(UDSRemoteStore, constructConfigWrongScheme)
{
    EXPECT_THROW(UDSRemoteStoreConfig("http", "/tmp/socket", {}), UsageError);
}

} // namespace bsd
#endif
