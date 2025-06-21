#include "bsd/util/config-global.hh"
#include "bsd/util/args.hh"
#include "bsd_api_util.h"
#include "bsd_api_util_internal.h"
#include "bsd/util/tests/bsd_api_util.hh"
#include "bsd/util/tests/string_callback.hh"

#include <gtest/gtest.h>

#include <memory>

#include "util-tests-config.hh"

namespace bsdC {

TEST_F(bsd_api_util_context, bsd_context_error)
{
    std::string err_msg_ref;
    try {
        throw bsd::Error("testing error");
    } catch (bsd::Error & e) {
        err_msg_ref = e.what();
        bsd_context_error(ctx);
    }
    ASSERT_EQ(ctx->last_err_code, NIX_ERR_NIX_ERROR);
    ASSERT_EQ(ctx->name, "bsd::Error");
    ASSERT_EQ(*ctx->last_err, err_msg_ref);
    ASSERT_EQ(ctx->info->msg.str(), "testing error");

    try {
        throw std::runtime_error("testing exception");
    } catch (std::exception & e) {
        err_msg_ref = e.what();
        bsd_context_error(ctx);
    }
    ASSERT_EQ(ctx->last_err_code, NIX_ERR_UNKNOWN);
    ASSERT_EQ(*ctx->last_err, err_msg_ref);

    bsd_clear_err(ctx);
    ASSERT_EQ(ctx->last_err_code, NIX_OK);
}

TEST_F(bsd_api_util_context, bsd_set_err_msg)
{
    ASSERT_EQ(ctx->last_err_code, NIX_OK);
    bsd_set_err_msg(ctx, NIX_ERR_UNKNOWN, "unknown test error");
    ASSERT_EQ(ctx->last_err_code, NIX_ERR_UNKNOWN);
    ASSERT_EQ(*ctx->last_err, "unknown test error");
}

TEST(bsd_api_util, bsd_version_get)
{
    ASSERT_EQ(std::string(bsd_version_get()), PACKAGE_VERSION);
}

struct MySettings : bsd::Config
{
    bsd::Setting<std::string> settingSet{this, "empty", "setting-name", "Description"};
};

MySettings mySettings;
static bsd::GlobalConfig::Register rs(&mySettings);

static auto createOwnedBsdContext()
{
    return std::unique_ptr<bsd_c_context, decltype([](bsd_c_context * ctx) {
                               if (ctx)
                                   bsd_c_context_free(ctx);
                           })>(bsd_c_context_create(), {});
}

TEST_F(bsd_api_util_context, bsd_setting_get)
{
    ASSERT_EQ(ctx->last_err_code, NIX_OK);
    std::string setting_value;
    bsd_err result = bsd_setting_get(ctx, "invalid-key", OBSERVE_STRING(setting_value));
    ASSERT_EQ(result, NIX_ERR_KEY);

    result = bsd_setting_get(ctx, "setting-name", OBSERVE_STRING(setting_value));
    ASSERT_EQ(result, NIX_OK);
    ASSERT_STREQ("empty", setting_value.c_str());
}

TEST_F(bsd_api_util_context, bsd_setting_set)
{
    bsd_err result = bsd_setting_set(ctx, "invalid-key", "new-value");
    ASSERT_EQ(result, NIX_ERR_KEY);

    result = bsd_setting_set(ctx, "setting-name", "new-value");
    ASSERT_EQ(result, NIX_OK);

    std::string setting_value;
    result = bsd_setting_get(ctx, "setting-name", OBSERVE_STRING(setting_value));
    ASSERT_EQ(result, NIX_OK);
    ASSERT_STREQ("new-value", setting_value.c_str());
}

TEST_F(bsd_api_util_context, bsd_err_msg)
{
    // no error
    EXPECT_THROW(bsd_err_msg(nullptr, ctx, NULL), bsd::Error);

    // set error
    bsd_set_err_msg(ctx, NIX_ERR_UNKNOWN, "unknown test error");

    // basic usage
    std::string err_msg = bsd_err_msg(NULL, ctx, NULL);
    ASSERT_EQ(err_msg, "unknown test error");

    // advanced usage
    unsigned int sz;
    auto new_ctx = createOwnedBsdContext();
    err_msg = bsd_err_msg(new_ctx.get(), ctx, &sz);
    ASSERT_EQ(sz, err_msg.size());
}

TEST_F(bsd_api_util_context, bsd_err_info_msg)
{
    std::string err_info;

    // no error
    EXPECT_THROW(bsd_err_info_msg(NULL, ctx, OBSERVE_STRING(err_info)), bsd::Error);

    try {
        throw bsd::Error("testing error");
    } catch (...) {
        bsd_context_error(ctx);
    }
    auto new_ctx = createOwnedBsdContext();
    bsd_err_info_msg(new_ctx.get(), ctx, OBSERVE_STRING(err_info));
    ASSERT_STREQ("testing error", err_info.c_str());
}

TEST_F(bsd_api_util_context, bsd_err_name)
{
    std::string err_name;

    // no error
    EXPECT_THROW(bsd_err_name(NULL, ctx, OBSERVE_STRING(err_name)), bsd::Error);

    try {
        throw bsd::Error("testing error");
    } catch (...) {
        bsd_context_error(ctx);
    }
    auto new_ctx = createOwnedBsdContext();
    bsd_err_name(new_ctx.get(), ctx, OBSERVE_STRING(err_name));
    ASSERT_EQ(std::string(err_name), "bsd::Error");
}

TEST_F(bsd_api_util_context, bsd_err_code)
{
    ASSERT_EQ(bsd_err_code(ctx), NIX_OK);
    bsd_set_err_msg(ctx, NIX_ERR_UNKNOWN, "unknown test error");
    ASSERT_EQ(bsd_err_code(ctx), NIX_ERR_UNKNOWN);
}

}
