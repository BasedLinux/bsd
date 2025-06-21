#include "bsd_api_store.h"
#include "bsd_api_store_internal.h"
#include "bsd_api_util.h"
#include "bsd_api_util_internal.h"
#include "bsd_api_expr.h"
#include "bsd_api_value.h"

#include "bsd/expr/tests/bsd_api_expr.hh"
#include "bsd/util/tests/string_callback.hh"
#include "bsd/util/file-system.hh"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "expr-tests-config.hh"

namespace bsdC {

TEST_F(bsd_api_store_test, bsd_eval_state_lookup_path)
{
    auto tmpDir = bsd::createTempDir();
    auto delTmpDir = std::make_unique<bsd::AutoDelete>(tmpDir, true);
    auto bsdpkgs = tmpDir + "/pkgs";
    auto bsdos = tmpDir + "/cfg";
    std::filesystem::create_directories(bsdpkgs);
    std::filesystem::create_directories(bsdos);

    std::string bsdpkgsEntry = "bsdpkgs=" + bsdpkgs;
    std::string bsdosEntry = "bsdos-config=" + bsdos;
    const char * lookupPath[] = {bsdpkgsEntry.c_str(), bsdosEntry.c_str(), nullptr};

    auto builder = bsd_eval_state_builder_new(ctx, store);
    assert_ctx_ok();

    ASSERT_EQ(NIX_OK, bsd_eval_state_builder_set_lookup_path(ctx, builder, lookupPath));
    assert_ctx_ok();

    auto state = bsd_eval_state_build(ctx, builder);
    assert_ctx_ok();

    bsd_eval_state_builder_free(builder);

    Value * value = bsd_alloc_value(ctx, state);
    bsd_expr_eval_from_string(ctx, state, "builtins.seq <bsdos-config> <bsdpkgs>", ".", value);
    assert_ctx_ok();

    ASSERT_EQ(bsd_get_type(ctx, value), NIX_TYPE_PATH);
    assert_ctx_ok();

    auto pathStr = bsd_get_path_string(ctx, value);
    assert_ctx_ok();
    ASSERT_EQ(0, strcmp(pathStr, bsdpkgs.c_str()));
}

TEST_F(bsd_api_expr_test, bsd_expr_eval_from_string)
{
    bsd_expr_eval_from_string(nullptr, state, "builtins.bsdVersion", ".", value);
    bsd_value_force(nullptr, state, value);
    std::string result;
    bsd_get_string(nullptr, value, OBSERVE_STRING(result));

    ASSERT_STREQ(PACKAGE_VERSION, result.c_str());
}

TEST_F(bsd_api_expr_test, bsd_expr_eval_add_numbers)
{
    bsd_expr_eval_from_string(nullptr, state, "1 + 1", ".", value);
    bsd_value_force(nullptr, state, value);
    auto result = bsd_get_int(nullptr, value);

    ASSERT_EQ(2, result);
}

TEST_F(bsd_api_expr_test, bsd_expr_eval_drv)
{
    auto expr = R"(derivation { name = "myname"; builder = "mybuilder"; system = "mysystem"; })";
    bsd_expr_eval_from_string(nullptr, state, expr, ".", value);
    ASSERT_EQ(NIX_TYPE_ATTRS, bsd_get_type(nullptr, value));

    EvalState * stateFn = bsd_state_create(nullptr, nullptr, store);
    bsd_value * valueFn = bsd_alloc_value(nullptr, state);
    bsd_expr_eval_from_string(nullptr, stateFn, "builtins.toString", ".", valueFn);
    ASSERT_EQ(NIX_TYPE_FUNCTION, bsd_get_type(nullptr, valueFn));

    EvalState * stateResult = bsd_state_create(nullptr, nullptr, store);
    bsd_value * valueResult = bsd_alloc_value(nullptr, stateResult);
    bsd_value_call(ctx, stateResult, valueFn, value, valueResult);
    ASSERT_EQ(NIX_TYPE_STRING, bsd_get_type(nullptr, valueResult));

    std::string p;
    bsd_get_string(nullptr, valueResult, OBSERVE_STRING(p));
    std::string pEnd = "-myname";
    ASSERT_EQ(pEnd, p.substr(p.size() - pEnd.size()));

    // Clean up
    bsd_gc_decref(nullptr, valueFn);
    bsd_state_free(stateFn);

    bsd_gc_decref(nullptr, valueResult);
    bsd_state_free(stateResult);
}

TEST_F(bsd_api_expr_test, bsd_build_drv)
{
    auto expr = R"(derivation { name = "myname";
                                system = builtins.currentSystem;
                                builder = "/bin/sh";
                                args = [ "-c" "echo foo > $out" ];
                              })";
    bsd_expr_eval_from_string(nullptr, state, expr, ".", value);

    bsd_value * drvPathValue = bsd_get_attr_byname(nullptr, value, state, "drvPath");
    std::string drvPath;
    bsd_get_string(nullptr, drvPathValue, OBSERVE_STRING(drvPath));

    std::string p = drvPath;
    std::string pEnd = "-myname.drv";
    ASSERT_EQ(pEnd, p.substr(p.size() - pEnd.size()));

    // NOTE: .drvPath should be usually be ignored. Output paths are more versatile.
    //       See https://github.com/BasedLinux/bsd/issues/6507
    //       Use e.g. bsd_string_realise to realise the output.
    StorePath * drvStorePath = bsd_store_parse_path(ctx, store, drvPath.c_str());
    ASSERT_EQ(true, bsd_store_is_valid_path(ctx, store, drvStorePath));

    bsd_value * outPathValue = bsd_get_attr_byname(ctx, value, state, "outPath");
    std::string outPath;
    bsd_get_string(ctx, outPathValue, OBSERVE_STRING(outPath));

    p = outPath;
    pEnd = "-myname";
    ASSERT_EQ(pEnd, p.substr(p.size() - pEnd.size()));
    ASSERT_EQ(true, drvStorePath->path.isDerivation());

    StorePath * outStorePath = bsd_store_parse_path(ctx, store, outPath.c_str());
    ASSERT_EQ(false, bsd_store_is_valid_path(ctx, store, outStorePath));

    bsd_store_realise(ctx, store, drvStorePath, nullptr, nullptr);
    auto is_valid_path = bsd_store_is_valid_path(ctx, store, outStorePath);
    ASSERT_EQ(true, is_valid_path);

    // Clean up
    bsd_store_path_free(drvStorePath);
    bsd_store_path_free(outStorePath);
}

TEST_F(bsd_api_expr_test, bsd_expr_realise_context_bad_value)
{
    auto expr = "true";
    bsd_expr_eval_from_string(ctx, state, expr, ".", value);
    assert_ctx_ok();
    auto r = bsd_string_realise(ctx, state, value, false);
    ASSERT_EQ(nullptr, r);
    ASSERT_EQ(ctx->last_err_code, NIX_ERR_NIX_ERROR);
    ASSERT_THAT(ctx->last_err, testing::Optional(testing::HasSubstr("cannot coerce")));
}

TEST_F(bsd_api_expr_test, bsd_expr_realise_context_bad_build)
{
    auto expr = R"(
        derivation { name = "letsbuild";
            system = builtins.currentSystem;
            builder = "/bin/sh";
            args = [ "-c" "echo failing a build for testing purposes; exit 1;" ];
            }
        )";
    bsd_expr_eval_from_string(ctx, state, expr, ".", value);
    assert_ctx_ok();
    auto r = bsd_string_realise(ctx, state, value, false);
    ASSERT_EQ(nullptr, r);
    ASSERT_EQ(ctx->last_err_code, NIX_ERR_NIX_ERROR);
    ASSERT_THAT(ctx->last_err, testing::Optional(testing::HasSubstr("failed with exit code 1")));
}

TEST_F(bsd_api_expr_test, bsd_expr_realise_context)
{
    // TODO (ca-derivations): add a content-addressing derivation output, which produces a placeholder
    auto expr = R"(
        ''
            a derivation output: ${
                derivation { name = "letsbuild";
                    system = builtins.currentSystem;
                    builder = "/bin/sh";
                    args = [ "-c" "echo foo > $out" ];
                    }}
            a path: ${builtins.toFile "just-a-file" "ooh file good"}
            a derivation path by itself: ${
                builtins.unsafeDiscardOutputDependency
                    (derivation {
                        name = "not-actually-built-yet";
                        system = builtins.currentSystem;
                        builder = "/bin/sh";
                        args = [ "-c" "echo foo > $out" ];
                    }).drvPath}
        ''
        )";
    bsd_expr_eval_from_string(ctx, state, expr, ".", value);
    assert_ctx_ok();
    auto r = bsd_string_realise(ctx, state, value, false);
    assert_ctx_ok();
    ASSERT_NE(nullptr, r);

    auto s = std::string(bsd_realised_string_get_buffer_start(r), bsd_realised_string_get_buffer_size(r));

    EXPECT_THAT(s, testing::StartsWith("a derivation output:"));
    EXPECT_THAT(s, testing::HasSubstr("-letsbuild\n"));
    EXPECT_THAT(s, testing::Not(testing::HasSubstr("-letsbuild.drv")));
    EXPECT_THAT(s, testing::HasSubstr("a path:"));
    EXPECT_THAT(s, testing::HasSubstr("-just-a-file"));
    EXPECT_THAT(s, testing::Not(testing::HasSubstr("-just-a-file.drv")));
    EXPECT_THAT(s, testing::Not(testing::HasSubstr("ooh file good")));
    EXPECT_THAT(s, testing::HasSubstr("a derivation path by itself:"));
    EXPECT_THAT(s, testing::EndsWith("-not-actually-built-yet.drv\n"));

    std::vector<std::string> names;
    size_t n = bsd_realised_string_get_store_path_count(r);
    for (size_t i = 0; i < n; ++i) {
        const StorePath * p = bsd_realised_string_get_store_path(r, i);
        ASSERT_NE(nullptr, p);
        std::string name;
        bsd_store_path_name(p, OBSERVE_STRING(name));
        names.push_back(name);
    }
    std::sort(names.begin(), names.end());
    ASSERT_EQ(3u, names.size());
    EXPECT_THAT(names[0], testing::StrEq("just-a-file"));
    EXPECT_THAT(names[1], testing::StrEq("letsbuild"));
    EXPECT_THAT(names[2], testing::StrEq("not-actually-built-yet.drv"));

    bsd_realised_string_free(r);
}

const char * SAMPLE_USER_DATA = "whatever";

static void
primop_square(void * user_data, bsd_c_context * context, EvalState * state, bsd_value ** args, bsd_value * ret)
{
    assert(context);
    assert(state);
    assert(user_data == SAMPLE_USER_DATA);
    auto i = bsd_get_int(context, args[0]);
    bsd_init_int(context, ret, i * i);
}

TEST_F(bsd_api_expr_test, bsd_expr_primop)
{
    PrimOp * primop =
        bsd_alloc_primop(ctx, primop_square, 1, "square", nullptr, "square an integer", (void *) SAMPLE_USER_DATA);
    assert_ctx_ok();
    bsd_value * primopValue = bsd_alloc_value(ctx, state);
    assert_ctx_ok();
    bsd_init_primop(ctx, primopValue, primop);
    assert_ctx_ok();

    bsd_value * three = bsd_alloc_value(ctx, state);
    assert_ctx_ok();
    bsd_init_int(ctx, three, 3);
    assert_ctx_ok();

    bsd_value * result = bsd_alloc_value(ctx, state);
    assert_ctx_ok();
    bsd_value_call(ctx, state, primopValue, three, result);
    assert_ctx_ok();

    auto r = bsd_get_int(ctx, result);
    ASSERT_EQ(9, r);
}

static void
primop_repeat(void * user_data, bsd_c_context * context, EvalState * state, bsd_value ** args, bsd_value * ret)
{
    assert(context);
    assert(state);
    assert(user_data == SAMPLE_USER_DATA);

    // Get the string to repeat
    std::string s;
    if (bsd_get_string(context, args[0], OBSERVE_STRING(s)) != NIX_OK)
        return;

    // Get the number of times to repeat
    auto n = bsd_get_int(context, args[1]);
    if (bsd_err_code(context) != NIX_OK)
        return;

    // Repeat the string
    std::string result;
    for (int i = 0; i < n; ++i)
        result += s;

    bsd_init_string(context, ret, result.c_str());
}

TEST_F(bsd_api_expr_test, bsd_expr_primop_arity_2_multiple_calls)
{
    PrimOp * primop =
        bsd_alloc_primop(ctx, primop_repeat, 2, "repeat", nullptr, "repeat a string", (void *) SAMPLE_USER_DATA);
    assert_ctx_ok();
    bsd_value * primopValue = bsd_alloc_value(ctx, state);
    assert_ctx_ok();
    bsd_init_primop(ctx, primopValue, primop);
    assert_ctx_ok();

    bsd_value * hello = bsd_alloc_value(ctx, state);
    assert_ctx_ok();
    bsd_init_string(ctx, hello, "hello");
    assert_ctx_ok();

    bsd_value * three = bsd_alloc_value(ctx, state);
    assert_ctx_ok();
    bsd_init_int(ctx, three, 3);
    assert_ctx_ok();

    bsd_value * partial = bsd_alloc_value(ctx, state);
    assert_ctx_ok();
    bsd_value_call(ctx, state, primopValue, hello, partial);
    assert_ctx_ok();

    bsd_value * result = bsd_alloc_value(ctx, state);
    assert_ctx_ok();
    bsd_value_call(ctx, state, partial, three, result);
    assert_ctx_ok();

    std::string r;
    bsd_get_string(ctx, result, OBSERVE_STRING(r));
    ASSERT_STREQ("hellohellohello", r.c_str());
}

TEST_F(bsd_api_expr_test, bsd_expr_primop_arity_2_single_call)
{
    PrimOp * primop =
        bsd_alloc_primop(ctx, primop_repeat, 2, "repeat", nullptr, "repeat a string", (void *) SAMPLE_USER_DATA);
    assert_ctx_ok();
    bsd_value * primopValue = bsd_alloc_value(ctx, state);
    assert_ctx_ok();
    bsd_init_primop(ctx, primopValue, primop);
    assert_ctx_ok();

    bsd_value * hello = bsd_alloc_value(ctx, state);
    assert_ctx_ok();
    bsd_init_string(ctx, hello, "hello");
    assert_ctx_ok();

    bsd_value * three = bsd_alloc_value(ctx, state);
    assert_ctx_ok();
    bsd_init_int(ctx, three, 3);
    assert_ctx_ok();

    bsd_value * result = bsd_alloc_value(ctx, state);
    assert_ctx_ok();
    NIX_VALUE_CALL(ctx, state, result, primopValue, hello, three);
    assert_ctx_ok();

    std::string r;
    bsd_get_string(ctx, result, OBSERVE_STRING(r));
    assert_ctx_ok();

    ASSERT_STREQ("hellohellohello", r.c_str());
}

static void
primop_bad_no_return(void * user_data, bsd_c_context * context, EvalState * state, bsd_value ** args, bsd_value * ret)
{
}

TEST_F(bsd_api_expr_test, bsd_expr_primop_bad_no_return)
{
    PrimOp * primop =
        bsd_alloc_primop(ctx, primop_bad_no_return, 1, "badNoReturn", nullptr, "a broken primop", nullptr);
    assert_ctx_ok();
    bsd_value * primopValue = bsd_alloc_value(ctx, state);
    assert_ctx_ok();
    bsd_init_primop(ctx, primopValue, primop);
    assert_ctx_ok();

    bsd_value * three = bsd_alloc_value(ctx, state);
    assert_ctx_ok();
    bsd_init_int(ctx, three, 3);
    assert_ctx_ok();

    bsd_value * result = bsd_alloc_value(ctx, state);
    assert_ctx_ok();
    bsd_value_call(ctx, state, primopValue, three, result);
    ASSERT_EQ(ctx->last_err_code, NIX_ERR_NIX_ERROR);
    ASSERT_THAT(
        ctx->last_err,
        testing::Optional(
            testing::HasSubstr("Implementation error in custom function: return value was not initialized")));
    ASSERT_THAT(ctx->last_err, testing::Optional(testing::HasSubstr("badNoReturn")));
}

static void primop_bad_return_thunk(
    void * user_data, bsd_c_context * context, EvalState * state, bsd_value ** args, bsd_value * ret)
{
    bsd_init_apply(context, ret, args[0], args[1]);
}
TEST_F(bsd_api_expr_test, bsd_expr_primop_bad_return_thunk)
{
    PrimOp * primop =
        bsd_alloc_primop(ctx, primop_bad_return_thunk, 2, "badReturnThunk", nullptr, "a broken primop", nullptr);
    assert_ctx_ok();
    bsd_value * primopValue = bsd_alloc_value(ctx, state);
    assert_ctx_ok();
    bsd_init_primop(ctx, primopValue, primop);
    assert_ctx_ok();

    bsd_value * toString = bsd_alloc_value(ctx, state);
    assert_ctx_ok();
    bsd_expr_eval_from_string(ctx, state, "builtins.toString", ".", toString);
    assert_ctx_ok();

    bsd_value * four = bsd_alloc_value(ctx, state);
    assert_ctx_ok();
    bsd_init_int(ctx, four, 4);
    assert_ctx_ok();

    bsd_value * result = bsd_alloc_value(ctx, state);
    assert_ctx_ok();
    NIX_VALUE_CALL(ctx, state, result, primopValue, toString, four);

    ASSERT_EQ(ctx->last_err_code, NIX_ERR_NIX_ERROR);
    ASSERT_THAT(
        ctx->last_err,
        testing::Optional(
            testing::HasSubstr("Implementation error in custom function: return value must not be a thunk")));
    ASSERT_THAT(ctx->last_err, testing::Optional(testing::HasSubstr("badReturnThunk")));
}

TEST_F(bsd_api_expr_test, bsd_value_call_multi_no_args)
{
    bsd_value * n = bsd_alloc_value(ctx, state);
    bsd_init_int(ctx, n, 3);
    assert_ctx_ok();

    bsd_value * r = bsd_alloc_value(ctx, state);
    bsd_value_call_multi(ctx, state, n, 0, nullptr, r);
    assert_ctx_ok();

    auto rInt = bsd_get_int(ctx, r);
    assert_ctx_ok();
    ASSERT_EQ(3, rInt);
}
} // namespace bsdC
