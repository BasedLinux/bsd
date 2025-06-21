#include "gmock/gmock.h"
#include <gtest/gtest.h>

#include "bsd_api_fetchers.h"
#include "bsd/store/tests/bsd_api_store.hh"

namespace bsdC {

TEST_F(bsd_api_store_test, bsd_api_fetchers_new_free)
{
    bsd_fetchers_settings * settings = bsd_fetchers_settings_new(ctx);
    assert_ctx_ok();
    ASSERT_NE(nullptr, settings);

    bsd_fetchers_settings_free(settings);
}

} // namespace bsdC
