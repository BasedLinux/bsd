#include "bsd/util/file-system.hh"
#include "bsd_api_store.h"
#include "bsd_api_util.h"
#include "bsd_api_expr.h"
#include "bsd_api_value.h"
#include "bsd_api_flake.h"

#include "bsd/expr/tests/bsd_api_expr.hh"
#include "bsd/util/tests/string_callback.hh"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace bsdC {

TEST_F(bsd_api_store_test, bsd_api_init_getFlake_exists)
{
    bsd_libstore_init(ctx);
    assert_ctx_ok();
    bsd_libexpr_init(ctx);
    assert_ctx_ok();

    auto settings = bsd_flake_settings_new(ctx);
    assert_ctx_ok();
    ASSERT_NE(nullptr, settings);

    bsd_eval_state_builder * builder = bsd_eval_state_builder_new(ctx, store);
    ASSERT_NE(nullptr, builder);
    assert_ctx_ok();

    bsd_flake_settings_add_to_eval_state_builder(ctx, settings, builder);
    assert_ctx_ok();

    auto state = bsd_eval_state_build(ctx, builder);
    assert_ctx_ok();
    ASSERT_NE(nullptr, state);

    bsd_eval_state_builder_free(builder);

    auto value = bsd_alloc_value(ctx, state);
    assert_ctx_ok();
    ASSERT_NE(nullptr, value);

    bsd_err err = bsd_expr_eval_from_string(ctx, state, "builtins.getFlake", ".", value);

    bsd_state_free(state);

    assert_ctx_ok();
    ASSERT_EQ(NIX_OK, err);
    ASSERT_EQ(NIX_TYPE_FUNCTION, bsd_get_type(ctx, value));
}

TEST_F(bsd_api_store_test, bsd_api_flake_reference_not_absolute_no_basedir_fail)
{
    bsd_libstore_init(ctx);
    assert_ctx_ok();
    bsd_libexpr_init(ctx);
    assert_ctx_ok();

    auto settings = bsd_flake_settings_new(ctx);
    assert_ctx_ok();
    ASSERT_NE(nullptr, settings);

    auto fetchSettings = bsd_fetchers_settings_new(ctx);
    assert_ctx_ok();
    ASSERT_NE(nullptr, fetchSettings);

    auto parseFlags = bsd_flake_reference_parse_flags_new(ctx, settings);

    std::string str(".#legacyPackages.aarch127-unknown...orion");
    std::string fragment;
    bsd_flake_reference * flakeReference = nullptr;
    auto r = bsd_flake_reference_and_fragment_from_string(
        ctx, fetchSettings, settings, parseFlags, str.data(), str.size(), &flakeReference, OBSERVE_STRING(fragment));

    ASSERT_NE(NIX_OK, r);
    ASSERT_EQ(nullptr, flakeReference);

    bsd_flake_reference_parse_flags_free(parseFlags);
}

TEST_F(bsd_api_store_test, bsd_api_load_flake)
{
    auto tmpDir = bsd::createTempDir();
    bsd::AutoDelete delTmpDir(tmpDir, true);

    bsd::writeFile(tmpDir + "/flake.bsd", R"(
        {
            outputs = { ... }: {
                hello = "potato";
            };
        }
    )");

    bsd_libstore_init(ctx);
    assert_ctx_ok();
    bsd_libexpr_init(ctx);
    assert_ctx_ok();

    auto fetchSettings = bsd_fetchers_settings_new(ctx);
    assert_ctx_ok();
    ASSERT_NE(nullptr, fetchSettings);

    auto settings = bsd_flake_settings_new(ctx);
    assert_ctx_ok();
    ASSERT_NE(nullptr, settings);

    bsd_eval_state_builder * builder = bsd_eval_state_builder_new(ctx, store);
    ASSERT_NE(nullptr, builder);
    assert_ctx_ok();

    auto state = bsd_eval_state_build(ctx, builder);
    assert_ctx_ok();
    ASSERT_NE(nullptr, state);

    bsd_eval_state_builder_free(builder);

    auto parseFlags = bsd_flake_reference_parse_flags_new(ctx, settings);
    assert_ctx_ok();
    ASSERT_NE(nullptr, parseFlags);

    auto r0 = bsd_flake_reference_parse_flags_set_base_directory(ctx, parseFlags, tmpDir.c_str(), tmpDir.size());
    assert_ctx_ok();
    ASSERT_EQ(NIX_OK, r0);

    std::string fragment;
    const std::string ref = ".#legacyPackages.aarch127-unknown...orion";
    bsd_flake_reference * flakeReference = nullptr;
    auto r = bsd_flake_reference_and_fragment_from_string(
        ctx, fetchSettings, settings, parseFlags, ref.data(), ref.size(), &flakeReference, OBSERVE_STRING(fragment));
    assert_ctx_ok();
    ASSERT_EQ(NIX_OK, r);
    ASSERT_NE(nullptr, flakeReference);
    ASSERT_EQ(fragment, "legacyPackages.aarch127-unknown...orion");

    bsd_flake_reference_parse_flags_free(parseFlags);

    auto lockFlags = bsd_flake_lock_flags_new(ctx, settings);
    assert_ctx_ok();
    ASSERT_NE(nullptr, lockFlags);

    auto lockedFlake = bsd_flake_lock(ctx, fetchSettings, settings, state, lockFlags, flakeReference);
    assert_ctx_ok();
    ASSERT_NE(nullptr, lockedFlake);

    bsd_flake_lock_flags_free(lockFlags);

    auto value = bsd_locked_flake_get_output_attrs(ctx, settings, state, lockedFlake);
    assert_ctx_ok();
    ASSERT_NE(nullptr, value);

    auto helloAttr = bsd_get_attr_byname(ctx, value, state, "hello");
    assert_ctx_ok();
    ASSERT_NE(nullptr, helloAttr);

    std::string helloStr;
    bsd_get_string(ctx, helloAttr, OBSERVE_STRING(helloStr));
    assert_ctx_ok();
    ASSERT_EQ("potato", helloStr);

    bsd_value_decref(ctx, value);
    bsd_locked_flake_free(lockedFlake);
    bsd_flake_reference_free(flakeReference);
    bsd_state_free(state);
    bsd_flake_settings_free(settings);
}

TEST_F(bsd_api_store_test, bsd_api_load_flake_with_flags)
{
    bsd_libstore_init(ctx);
    assert_ctx_ok();
    bsd_libexpr_init(ctx);
    assert_ctx_ok();

    auto tmpDir = bsd::createTempDir();
    bsd::AutoDelete delTmpDir(tmpDir, true);

    bsd::createDirs(tmpDir + "/b");
    bsd::writeFile(tmpDir + "/b/flake.bsd", R"(
        {
            outputs = { ... }: {
                hello = "BOB";
            };
        }
    )");

    bsd::createDirs(tmpDir + "/a");
    bsd::writeFile(tmpDir + "/a/flake.bsd", R"(
        {
            inputs.b.url = ")" + tmpDir + R"(/b";
            outputs = { b, ... }: {
                hello = b.hello;
            };
        }
    )");

    bsd::createDirs(tmpDir + "/c");
    bsd::writeFile(tmpDir + "/c/flake.bsd", R"(
        {
            outputs = { ... }: {
                hello = "Claire";
            };
        }
    )");

    bsd_libstore_init(ctx);
    assert_ctx_ok();

    auto fetchSettings = bsd_fetchers_settings_new(ctx);
    assert_ctx_ok();
    ASSERT_NE(nullptr, fetchSettings);

    auto settings = bsd_flake_settings_new(ctx);
    assert_ctx_ok();
    ASSERT_NE(nullptr, settings);

    bsd_eval_state_builder * builder = bsd_eval_state_builder_new(ctx, store);
    ASSERT_NE(nullptr, builder);
    assert_ctx_ok();

    auto state = bsd_eval_state_build(ctx, builder);
    assert_ctx_ok();
    ASSERT_NE(nullptr, state);

    bsd_eval_state_builder_free(builder);

    auto parseFlags = bsd_flake_reference_parse_flags_new(ctx, settings);
    assert_ctx_ok();
    ASSERT_NE(nullptr, parseFlags);

    auto r0 = bsd_flake_reference_parse_flags_set_base_directory(ctx, parseFlags, tmpDir.c_str(), tmpDir.size());
    assert_ctx_ok();
    ASSERT_EQ(NIX_OK, r0);

    std::string fragment;
    const std::string ref = "./a";
    bsd_flake_reference * flakeReference = nullptr;
    auto r = bsd_flake_reference_and_fragment_from_string(
        ctx, fetchSettings, settings, parseFlags, ref.data(), ref.size(), &flakeReference, OBSERVE_STRING(fragment));
    assert_ctx_ok();
    ASSERT_EQ(NIX_OK, r);
    ASSERT_NE(nullptr, flakeReference);
    ASSERT_EQ(fragment, "");

    // Step 1: Do not update, fails

    auto lockFlags = bsd_flake_lock_flags_new(ctx, settings);
    assert_ctx_ok();
    ASSERT_NE(nullptr, lockFlags);

    bsd_flake_lock_flags_set_mode_check(ctx, lockFlags);
    assert_ctx_ok();

    // Step 2: Update but do not write, succeeds

    auto lockedFlake = bsd_flake_lock(ctx, fetchSettings, settings, state, lockFlags, flakeReference);
    assert_ctx_err();
    ASSERT_EQ(nullptr, lockedFlake);

    bsd_flake_lock_flags_set_mode_virtual(ctx, lockFlags);
    assert_ctx_ok();

    lockedFlake = bsd_flake_lock(ctx, fetchSettings, settings, state, lockFlags, flakeReference);
    assert_ctx_ok();
    ASSERT_NE(nullptr, lockedFlake);

    // Get the output attrs
    auto value = bsd_locked_flake_get_output_attrs(ctx, settings, state, lockedFlake);
    assert_ctx_ok();
    ASSERT_NE(nullptr, value);

    auto helloAttr = bsd_get_attr_byname(ctx, value, state, "hello");
    assert_ctx_ok();
    ASSERT_NE(nullptr, helloAttr);

    std::string helloStr;
    bsd_get_string(ctx, helloAttr, OBSERVE_STRING(helloStr));
    assert_ctx_ok();
    ASSERT_EQ("BOB", helloStr);

    bsd_value_decref(ctx, value);
    bsd_locked_flake_free(lockedFlake);

    // Step 3: Lock was not written, so Step 1 would fail again

    bsd_flake_lock_flags_set_mode_check(ctx, lockFlags);

    lockedFlake = bsd_flake_lock(ctx, fetchSettings, settings, state, lockFlags, flakeReference);
    assert_ctx_err();
    ASSERT_EQ(nullptr, lockedFlake);

    // Step 4: Update and write, succeeds

    bsd_flake_lock_flags_set_mode_write_as_needed(ctx, lockFlags);
    assert_ctx_ok();

    lockedFlake = bsd_flake_lock(ctx, fetchSettings, settings, state, lockFlags, flakeReference);
    assert_ctx_ok();
    ASSERT_NE(nullptr, lockedFlake);

    // Get the output attrs
    value = bsd_locked_flake_get_output_attrs(ctx, settings, state, lockedFlake);
    assert_ctx_ok();
    ASSERT_NE(nullptr, value);

    helloAttr = bsd_get_attr_byname(ctx, value, state, "hello");
    assert_ctx_ok();
    ASSERT_NE(nullptr, helloAttr);

    helloStr.clear();
    bsd_get_string(ctx, helloAttr, OBSERVE_STRING(helloStr));
    assert_ctx_ok();
    ASSERT_EQ("BOB", helloStr);

    bsd_value_decref(ctx, value);
    bsd_locked_flake_free(lockedFlake);

    // Step 5: Lock was written, so Step 1 would succeed

    bsd_flake_lock_flags_set_mode_check(ctx, lockFlags);
    assert_ctx_ok();

    lockedFlake = bsd_flake_lock(ctx, fetchSettings, settings, state, lockFlags, flakeReference);
    assert_ctx_ok();
    ASSERT_NE(nullptr, lockedFlake);

    // Get the output attrs
    value = bsd_locked_flake_get_output_attrs(ctx, settings, state, lockedFlake);
    assert_ctx_ok();
    ASSERT_NE(nullptr, value);

    helloAttr = bsd_get_attr_byname(ctx, value, state, "hello");
    assert_ctx_ok();
    ASSERT_NE(nullptr, helloAttr);

    helloStr.clear();
    bsd_get_string(ctx, helloAttr, OBSERVE_STRING(helloStr));
    assert_ctx_ok();
    ASSERT_EQ("BOB", helloStr);

    bsd_value_decref(ctx, value);
    bsd_locked_flake_free(lockedFlake);

    // Step 6: Lock with override, do not write

    bsd_flake_lock_flags_set_mode_write_as_needed(ctx, lockFlags);
    assert_ctx_ok();

    bsd_flake_reference * overrideFlakeReference = nullptr;
    bsd_flake_reference_and_fragment_from_string(
        ctx, fetchSettings, settings, parseFlags, "./c", 3, &overrideFlakeReference, OBSERVE_STRING(fragment));
    assert_ctx_ok();
    ASSERT_NE(nullptr, overrideFlakeReference);

    bsd_flake_lock_flags_add_input_override(ctx, lockFlags, "b", overrideFlakeReference);
    assert_ctx_ok();

    lockedFlake = bsd_flake_lock(ctx, fetchSettings, settings, state, lockFlags, flakeReference);
    assert_ctx_ok();
    ASSERT_NE(nullptr, lockedFlake);

    // Get the output attrs
    value = bsd_locked_flake_get_output_attrs(ctx, settings, state, lockedFlake);
    assert_ctx_ok();
    ASSERT_NE(nullptr, value);

    helloAttr = bsd_get_attr_byname(ctx, value, state, "hello");
    assert_ctx_ok();
    ASSERT_NE(nullptr, helloAttr);

    helloStr.clear();
    bsd_get_string(ctx, helloAttr, OBSERVE_STRING(helloStr));
    assert_ctx_ok();
    ASSERT_EQ("Claire", helloStr);

    bsd_flake_reference_parse_flags_free(parseFlags);
    bsd_flake_lock_flags_free(lockFlags);
    bsd_flake_reference_free(flakeReference);
    bsd_state_free(state);
    bsd_flake_settings_free(settings);
}

} // namespace bsdC
