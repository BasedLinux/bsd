#include <gtest/gtest.h>

#include "bsd/store/legacy-ssh-store.hh"

namespace bsd {

TEST(LegacySSHStore, constructConfig)
{
    LegacySSHStoreConfig config{
        "ssh",
        "localhost",
        StoreConfig::Params{
            {
                "remote-program",
                // TODO #11106, no more split on space
                "foo bar",
            },
        }};
    EXPECT_EQ(
        config.remoteProgram.get(),
        (Strings{
            "foo",
            "bar",
        }));
}
}
