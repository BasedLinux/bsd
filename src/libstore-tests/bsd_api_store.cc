#include "bsd_api_util.h"
#include "bsd_api_util_internal.h"
#include "bsd_api_store.h"
#include "bsd_api_store_internal.h"

#include "bsd/store/tests/bsd_api_store.hh"
#include "bsd/util/tests/string_callback.hh"

#include "store-tests-config.hh"

namespace bsdC {

std::string PATH_SUFFIX = "/g1w7hy3qg1w7hy3qg1w7hy3qg1w7hy3q-name";

TEST_F(bsd_api_util_context, bsd_libstore_init)
{
    auto ret = bsd_libstore_init(ctx);
    ASSERT_EQ(NIX_OK, ret);
}

TEST_F(bsd_api_store_test, bsd_store_get_uri)
{
    std::string str;
    auto ret = bsd_store_get_uri(ctx, store, OBSERVE_STRING(str));
    ASSERT_EQ(NIX_OK, ret);
    ASSERT_STREQ("local", str.c_str());
}

TEST_F(bsd_api_util_context, bsd_store_get_storedir_default)
{
    if (bsd::getEnv("HOME").value_or("") == "/homeless-shelter") {
        // skipping test in sandbox because bsd_store_open tries to create /bsd/var/bsd/profiles
        GTEST_SKIP();
    }
    bsd_libstore_init(ctx);
    Store * store = bsd_store_open(ctx, nullptr, nullptr);
    assert_ctx_ok();
    ASSERT_NE(store, nullptr);

    std::string str;
    auto ret = bsd_store_get_storedir(ctx, store, OBSERVE_STRING(str));
    assert_ctx_ok();
    ASSERT_EQ(NIX_OK, ret);

    // These tests run with a unique storeDir, but not a relocated store
    ASSERT_STREQ(NIX_STORE_DIR, str.c_str());

    bsd_store_free(store);
}

TEST_F(bsd_api_store_test, bsd_store_get_storedir)
{
    std::string str;
    auto ret = bsd_store_get_storedir(ctx, store, OBSERVE_STRING(str));
    assert_ctx_ok();
    ASSERT_EQ(NIX_OK, ret);

    // These tests run with a unique storeDir, but not a relocated store
    ASSERT_STREQ(bsdStoreDir.c_str(), str.c_str());
}

TEST_F(bsd_api_store_test, InvalidPathFails)
{
    bsd_store_parse_path(ctx, store, "invalid-path");
    ASSERT_EQ(ctx->last_err_code, NIX_ERR_NIX_ERROR);
}

TEST_F(bsd_api_store_test, ReturnsValidStorePath)
{
    StorePath * result = bsd_store_parse_path(ctx, store, (bsdStoreDir + PATH_SUFFIX).c_str());
    ASSERT_NE(result, nullptr);
    ASSERT_STREQ("name", result->path.name().data());
    ASSERT_STREQ(PATH_SUFFIX.substr(1).c_str(), result->path.to_string().data());
}

TEST_F(bsd_api_store_test, SetsLastErrCodeToBsdOk)
{
    bsd_store_parse_path(ctx, store, (bsdStoreDir + PATH_SUFFIX).c_str());
    ASSERT_EQ(ctx->last_err_code, NIX_OK);
}

TEST_F(bsd_api_store_test, DoesNotCrashWhenContextIsNull)
{
    ASSERT_NO_THROW(bsd_store_parse_path(ctx, store, (bsdStoreDir + PATH_SUFFIX).c_str()));
}

TEST_F(bsd_api_store_test, get_version)
{
    std::string str;
    auto ret = bsd_store_get_version(ctx, store, OBSERVE_STRING(str));
    ASSERT_EQ(NIX_OK, ret);
    ASSERT_STREQ(PACKAGE_VERSION, str.c_str());
}

TEST_F(bsd_api_util_context, bsd_store_open_dummy)
{
    bsd_libstore_init(ctx);
    Store * store = bsd_store_open(ctx, "dummy://", nullptr);
    ASSERT_EQ(NIX_OK, ctx->last_err_code);
    ASSERT_STREQ("dummy", store->ptr->getUri().c_str());

    std::string str;
    bsd_store_get_version(ctx, store, OBSERVE_STRING(str));
    ASSERT_STREQ("", str.c_str());

    bsd_store_free(store);
}

TEST_F(bsd_api_util_context, bsd_store_open_invalid)
{
    bsd_libstore_init(ctx);
    Store * store = bsd_store_open(ctx, "invalid://", nullptr);
    ASSERT_EQ(NIX_ERR_NIX_ERROR, ctx->last_err_code);
    ASSERT_EQ(nullptr, store);
    bsd_store_free(store);
}

TEST_F(bsd_api_store_test, bsd_store_is_valid_path_not_in_store)
{
    StorePath * path = bsd_store_parse_path(ctx, store, (bsdStoreDir + PATH_SUFFIX).c_str());
    ASSERT_EQ(false, bsd_store_is_valid_path(ctx, store, path));
}

TEST_F(bsd_api_store_test, bsd_store_real_path)
{
    StorePath * path = bsd_store_parse_path(ctx, store, (bsdStoreDir + PATH_SUFFIX).c_str());
    std::string rp;
    auto ret = bsd_store_real_path(ctx, store, path, OBSERVE_STRING(rp));
    assert_ctx_ok();
    ASSERT_EQ(NIX_OK, ret);
    // Assumption: we're not testing with a relocated store
    ASSERT_STREQ((bsdStoreDir + PATH_SUFFIX).c_str(), rp.c_str());

    bsd_store_path_free(path);
}

TEST_F(bsd_api_util_context, bsd_store_real_path_relocated)
{
    if (bsd::getEnv("HOME").value_or("") == "/homeless-shelter") {
        // Can't open default store from within sandbox
        GTEST_SKIP();
    }
    auto tmp = bsd::createTempDir();
    std::string storeRoot = tmp + "/store";
    std::string stateDir = tmp + "/state";
    std::string logDir = tmp + "/log";
    const char * rootkv[] = {"root", storeRoot.c_str()};
    const char * statekv[] = {"state", stateDir.c_str()};
    const char * logkv[] = {"log", logDir.c_str()};
    // const char * rokv[] = {"read-only", "true"};
    const char ** kvs[] = {rootkv, statekv, logkv, NULL};

    bsd_libstore_init(ctx);
    assert_ctx_ok();

    Store * store = bsd_store_open(ctx, "local", kvs);
    assert_ctx_ok();
    ASSERT_NE(store, nullptr);

    std::string bsdStoreDir;
    auto ret = bsd_store_get_storedir(ctx, store, OBSERVE_STRING(bsdStoreDir));
    ASSERT_EQ(NIX_OK, ret);
    ASSERT_STREQ(NIX_STORE_DIR, bsdStoreDir.c_str());

    StorePath * path = bsd_store_parse_path(ctx, store, (bsdStoreDir + PATH_SUFFIX).c_str());
    assert_ctx_ok();
    ASSERT_NE(path, nullptr);

    std::string rp;
    ret = bsd_store_real_path(ctx, store, path, OBSERVE_STRING(rp));
    assert_ctx_ok();
    ASSERT_EQ(NIX_OK, ret);

    // Assumption: we're not testing with a relocated store
    ASSERT_STREQ((storeRoot + NIX_STORE_DIR + PATH_SUFFIX).c_str(), rp.c_str());

    bsd_store_path_free(path);
}

TEST_F(bsd_api_util_context, bsd_store_real_path_binary_cache)
{
    if (bsd::getEnv("HOME").value_or("") == "/homeless-shelter") {
        // TODO: override NIX_CACHE_HOME?
        // skipping test in sandbox because narinfo cache can't be written
        GTEST_SKIP();
    }

    Store * store = bsd_store_open(ctx, "https://cache.basedlinux.org", nullptr);
    assert_ctx_ok();
    ASSERT_NE(store, nullptr);

    std::string path_raw = std::string(NIX_STORE_DIR) + PATH_SUFFIX;
    StorePath * path = bsd_store_parse_path(ctx, store, path_raw.c_str());
    assert_ctx_ok();
    ASSERT_NE(path, nullptr);

    std::string rp;
    auto ret = bsd_store_real_path(ctx, store, path, OBSERVE_STRING(rp));
    assert_ctx_ok();
    ASSERT_EQ(NIX_OK, ret);
    ASSERT_STREQ(path_raw.c_str(), rp.c_str());
}

} // namespace bsdC
