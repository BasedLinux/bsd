#include "bsd_api_store.h"
#include "bsd_api_store_internal.h"
#include "bsd_api_util.h"
#include "bsd_api_util_internal.h"
#include "bsd_api_expr.h"
#include "bsd_api_value.h"
#include "bsd_api_expr_internal.h"

#include "bsd/expr/tests/bsd_api_expr.hh"
#include "bsd/util/tests/string_callback.hh"

#include <gmock/gmock.h>
#include <cstddef>
#include <cstdlib>
#include <gtest/gtest.h>

namespace bsdC {

TEST_F(bsd_api_expr_test, as_bsd_value_ptr)
{
    // bsd_alloc_value casts bsd::Value to bsd_value
    // It should be obvious from the decl that that works, but if it doesn't,
    // the whole implementation would be utterly broken.
    ASSERT_EQ(sizeof(bsd::Value), sizeof(bsd_value));
}

TEST_F(bsd_api_expr_test, bsd_value_get_int_invalid)
{
    ASSERT_EQ(0, bsd_get_int(ctx, nullptr));
    assert_ctx_err();
    ASSERT_EQ(0, bsd_get_int(ctx, value));
    assert_ctx_err();
}

TEST_F(bsd_api_expr_test, bsd_value_set_get_int)
{
    int myInt = 1;
    bsd_init_int(ctx, value, myInt);

    ASSERT_EQ(myInt, bsd_get_int(ctx, value));
    ASSERT_STREQ("an integer", bsd_get_typename(ctx, value));
    ASSERT_EQ(NIX_TYPE_INT, bsd_get_type(ctx, value));
}

TEST_F(bsd_api_expr_test, bsd_value_set_get_float_invalid)
{
    ASSERT_DOUBLE_EQ(0.0, bsd_get_float(ctx, nullptr));
    assert_ctx_err();
    ASSERT_DOUBLE_EQ(0.0, bsd_get_float(ctx, value));
    assert_ctx_err();
}

TEST_F(bsd_api_expr_test, bsd_value_set_get_float)
{
    double myDouble = 1.0;
    bsd_init_float(ctx, value, myDouble);

    ASSERT_DOUBLE_EQ(myDouble, bsd_get_float(ctx, value));
    ASSERT_STREQ("a float", bsd_get_typename(ctx, value));
    ASSERT_EQ(NIX_TYPE_FLOAT, bsd_get_type(ctx, value));
}

TEST_F(bsd_api_expr_test, bsd_value_set_get_bool_invalid)
{
    ASSERT_EQ(false, bsd_get_bool(ctx, nullptr));
    assert_ctx_err();
    ASSERT_EQ(false, bsd_get_bool(ctx, value));
    assert_ctx_err();
}

TEST_F(bsd_api_expr_test, bsd_value_set_get_bool)
{
    bool myBool = true;
    bsd_init_bool(ctx, value, myBool);

    ASSERT_EQ(myBool, bsd_get_bool(ctx, value));
    ASSERT_STREQ("a Boolean", bsd_get_typename(ctx, value));
    ASSERT_EQ(NIX_TYPE_BOOL, bsd_get_type(ctx, value));
}

TEST_F(bsd_api_expr_test, bsd_value_set_get_string_invalid)
{
    std::string string_value;
    ASSERT_EQ(NIX_ERR_UNKNOWN, bsd_get_string(ctx, nullptr, OBSERVE_STRING(string_value)));
    assert_ctx_err();
    ASSERT_EQ(NIX_ERR_UNKNOWN, bsd_get_string(ctx, value, OBSERVE_STRING(string_value)));
    assert_ctx_err();
}

TEST_F(bsd_api_expr_test, bsd_value_set_get_string)
{
    std::string string_value;
    const char * myString = "some string";
    bsd_init_string(ctx, value, myString);

    bsd_get_string(ctx, value, OBSERVE_STRING(string_value));
    ASSERT_STREQ(myString, string_value.c_str());
    ASSERT_STREQ("a string", bsd_get_typename(ctx, value));
    ASSERT_EQ(NIX_TYPE_STRING, bsd_get_type(ctx, value));
}

TEST_F(bsd_api_expr_test, bsd_value_set_get_null_invalid)
{
    ASSERT_EQ(NULL, bsd_get_typename(ctx, value));
    assert_ctx_err();
}

TEST_F(bsd_api_expr_test, bsd_value_set_get_null)
{
    bsd_init_null(ctx, value);

    ASSERT_STREQ("null", bsd_get_typename(ctx, value));
    ASSERT_EQ(NIX_TYPE_NULL, bsd_get_type(ctx, value));
}

TEST_F(bsd_api_expr_test, bsd_value_set_get_path_invalid)
{
    ASSERT_EQ(nullptr, bsd_get_path_string(ctx, nullptr));
    assert_ctx_err();
    ASSERT_EQ(nullptr, bsd_get_path_string(ctx, value));
    assert_ctx_err();
}
TEST_F(bsd_api_expr_test, bsd_value_set_get_path)
{
    const char * p = "/bsd/store/40s0qmrfb45vlh6610rk29ym318dswdr-myname";
    bsd_init_path_string(ctx, state, value, p);

    ASSERT_STREQ(p, bsd_get_path_string(ctx, value));
    ASSERT_STREQ("a path", bsd_get_typename(ctx, value));
    ASSERT_EQ(NIX_TYPE_PATH, bsd_get_type(ctx, value));
}

TEST_F(bsd_api_expr_test, bsd_build_and_init_list_invalid)
{
    ASSERT_EQ(nullptr, bsd_get_list_byidx(ctx, nullptr, state, 0));
    assert_ctx_err();
    ASSERT_EQ(0u, bsd_get_list_size(ctx, nullptr));
    assert_ctx_err();

    ASSERT_EQ(nullptr, bsd_get_list_byidx(ctx, value, state, 0));
    assert_ctx_err();
    ASSERT_EQ(0u, bsd_get_list_size(ctx, value));
    assert_ctx_err();
}

TEST_F(bsd_api_expr_test, bsd_build_and_init_list)
{
    int size = 10;
    ListBuilder * builder = bsd_make_list_builder(ctx, state, size);

    bsd_value * intValue = bsd_alloc_value(ctx, state);
    bsd_value * intValue2 = bsd_alloc_value(ctx, state);

    // `init` and `insert` can be called in any order
    bsd_init_int(ctx, intValue, 42);
    bsd_list_builder_insert(ctx, builder, 0, intValue);
    bsd_list_builder_insert(ctx, builder, 1, intValue2);
    bsd_init_int(ctx, intValue2, 43);

    bsd_make_list(ctx, builder, value);
    bsd_list_builder_free(builder);

    ASSERT_EQ(42, bsd_get_int(ctx, bsd_get_list_byidx(ctx, value, state, 0)));
    ASSERT_EQ(43, bsd_get_int(ctx, bsd_get_list_byidx(ctx, value, state, 1)));
    ASSERT_EQ(nullptr, bsd_get_list_byidx(ctx, value, state, 2));
    ASSERT_EQ(10u, bsd_get_list_size(ctx, value));

    ASSERT_STREQ("a list", bsd_get_typename(ctx, value));
    ASSERT_EQ(NIX_TYPE_LIST, bsd_get_type(ctx, value));

    // Clean up
    bsd_gc_decref(ctx, intValue);
}

TEST_F(bsd_api_expr_test, bsd_build_and_init_attr_invalid)
{
    ASSERT_EQ(nullptr, bsd_get_attr_byname(ctx, nullptr, state, 0));
    assert_ctx_err();
    ASSERT_EQ(nullptr, bsd_get_attr_byidx(ctx, nullptr, state, 0, nullptr));
    assert_ctx_err();
    ASSERT_EQ(nullptr, bsd_get_attr_name_byidx(ctx, nullptr, state, 0));
    assert_ctx_err();
    ASSERT_EQ(0u, bsd_get_attrs_size(ctx, nullptr));
    assert_ctx_err();
    ASSERT_EQ(false, bsd_has_attr_byname(ctx, nullptr, state, "no-value"));
    assert_ctx_err();

    ASSERT_EQ(nullptr, bsd_get_attr_byname(ctx, value, state, 0));
    assert_ctx_err();
    ASSERT_EQ(nullptr, bsd_get_attr_byidx(ctx, value, state, 0, nullptr));
    assert_ctx_err();
    ASSERT_EQ(nullptr, bsd_get_attr_name_byidx(ctx, value, state, 0));
    assert_ctx_err();
    ASSERT_EQ(0u, bsd_get_attrs_size(ctx, value));
    assert_ctx_err();
    ASSERT_EQ(false, bsd_has_attr_byname(ctx, value, state, "no-value"));
    assert_ctx_err();
}

TEST_F(bsd_api_expr_test, bsd_build_and_init_attr)
{
    int size = 10;
    const char ** out_name = (const char **) malloc(sizeof(char *));

    BindingsBuilder * builder = bsd_make_bindings_builder(ctx, state, size);

    bsd_value * intValue = bsd_alloc_value(ctx, state);
    bsd_init_int(ctx, intValue, 42);

    bsd_value * stringValue = bsd_alloc_value(ctx, state);
    bsd_init_string(ctx, stringValue, "foo");

    bsd_bindings_builder_insert(ctx, builder, "a", intValue);
    bsd_bindings_builder_insert(ctx, builder, "b", stringValue);
    bsd_make_attrs(ctx, value, builder);
    bsd_bindings_builder_free(builder);

    ASSERT_EQ(2u, bsd_get_attrs_size(ctx, value));

    bsd_value * out_value = bsd_get_attr_byname(ctx, value, state, "a");
    ASSERT_EQ(42, bsd_get_int(ctx, out_value));
    bsd_gc_decref(ctx, out_value);

    out_value = bsd_get_attr_byidx(ctx, value, state, 0, out_name);
    ASSERT_EQ(42, bsd_get_int(ctx, out_value));
    ASSERT_STREQ("a", *out_name);
    bsd_gc_decref(ctx, out_value);

    ASSERT_STREQ("a", bsd_get_attr_name_byidx(ctx, value, state, 0));

    ASSERT_EQ(true, bsd_has_attr_byname(ctx, value, state, "b"));
    ASSERT_EQ(false, bsd_has_attr_byname(ctx, value, state, "no-value"));

    out_value = bsd_get_attr_byname(ctx, value, state, "b");
    std::string string_value;
    bsd_get_string(ctx, out_value, OBSERVE_STRING(string_value));
    ASSERT_STREQ("foo", string_value.c_str());
    bsd_gc_decref(nullptr, out_value);

    out_value = bsd_get_attr_byidx(ctx, value, state, 1, out_name);
    bsd_get_string(ctx, out_value, OBSERVE_STRING(string_value));
    ASSERT_STREQ("foo", string_value.c_str());
    ASSERT_STREQ("b", *out_name);
    bsd_gc_decref(nullptr, out_value);

    ASSERT_STREQ("b", bsd_get_attr_name_byidx(ctx, value, state, 1));

    ASSERT_STREQ("a set", bsd_get_typename(ctx, value));
    ASSERT_EQ(NIX_TYPE_ATTRS, bsd_get_type(ctx, value));

    // Clean up
    bsd_gc_decref(ctx, intValue);
    bsd_gc_decref(ctx, stringValue);
    free(out_name);
}

TEST_F(bsd_api_expr_test, bsd_value_init)
{
    // Setup

    // two = 2;
    // f = a: a * a;

    bsd_value * two = bsd_alloc_value(ctx, state);
    bsd_init_int(ctx, two, 2);

    bsd_value * f = bsd_alloc_value(ctx, state);
    bsd_expr_eval_from_string(
        ctx,
        state,
        R"(
        a: a * a
    )",
        "<test>",
        f);

    // Test

    // r = f two;

    bsd_value * r = bsd_alloc_value(ctx, state);
    bsd_init_apply(ctx, r, f, two);
    assert_ctx_ok();

    ValueType t = bsd_get_type(ctx, r);
    assert_ctx_ok();

    ASSERT_EQ(t, NIX_TYPE_THUNK);

    bsd_value_force(ctx, state, r);

    t = bsd_get_type(ctx, r);
    assert_ctx_ok();

    ASSERT_EQ(t, NIX_TYPE_INT);

    int n = bsd_get_int(ctx, r);
    assert_ctx_ok();

    ASSERT_EQ(n, 4);

    // Clean up
    bsd_gc_decref(ctx, two);
    bsd_gc_decref(ctx, f);
    bsd_gc_decref(ctx, r);
}

TEST_F(bsd_api_expr_test, bsd_value_init_apply_error)
{
    bsd_value * some_string = bsd_alloc_value(ctx, state);
    bsd_init_string(ctx, some_string, "some string");
    assert_ctx_ok();

    bsd_value * v = bsd_alloc_value(ctx, state);
    bsd_init_apply(ctx, v, some_string, some_string);
    assert_ctx_ok();

    // All ok. Call has not been evaluated yet.

    // Evaluate it
    bsd_value_force(ctx, state, v);
    ASSERT_EQ(ctx->last_err_code, NIX_ERR_NIX_ERROR);
    ASSERT_THAT(ctx->last_err.value(), testing::HasSubstr("attempt to call something which is not a function but"));

    // Clean up
    bsd_gc_decref(ctx, some_string);
    bsd_gc_decref(ctx, v);
}

TEST_F(bsd_api_expr_test, bsd_value_init_apply_lazy_arg)
{
    // f is a lazy function: it does not evaluate its argument before returning its return value
    // g is a helper to produce e
    // e is a thunk that throws an exception
    //
    // r = f e
    // r should not throw an exception, because e is not evaluated

    bsd_value * f = bsd_alloc_value(ctx, state);
    bsd_expr_eval_from_string(
        ctx,
        state,
        R"(
        a: { foo = a; }
    )",
        "<test>",
        f);
    assert_ctx_ok();

    bsd_value * e = bsd_alloc_value(ctx, state);
    {
        bsd_value * g = bsd_alloc_value(ctx, state);
        bsd_expr_eval_from_string(
            ctx,
            state,
            R"(
            _ignore: throw "error message for test case bsd_value_init_apply_lazy_arg"
        )",
            "<test>",
            g);
        assert_ctx_ok();

        bsd_init_apply(ctx, e, g, g);
        assert_ctx_ok();
        bsd_gc_decref(ctx, g);
    }

    bsd_value * r = bsd_alloc_value(ctx, state);
    bsd_init_apply(ctx, r, f, e);
    assert_ctx_ok();

    bsd_value_force(ctx, state, r);
    assert_ctx_ok();

    auto n = bsd_get_attrs_size(ctx, r);
    assert_ctx_ok();
    ASSERT_EQ(1u, n);

    // bsd_get_attr_byname isn't lazy (it could have been) so it will throw the exception
    bsd_value * foo = bsd_get_attr_byname(ctx, r, state, "foo");
    ASSERT_EQ(nullptr, foo);
    ASSERT_THAT(ctx->last_err.value(), testing::HasSubstr("error message for test case bsd_value_init_apply_lazy_arg"));

    // Clean up
    bsd_gc_decref(ctx, f);
    bsd_gc_decref(ctx, e);
}

TEST_F(bsd_api_expr_test, bsd_copy_value)
{
    bsd_value * source = bsd_alloc_value(ctx, state);

    bsd_init_int(ctx, source, 42);
    bsd_copy_value(ctx, value, source);

    ASSERT_EQ(42, bsd_get_int(ctx, value));

    // Clean up
    bsd_gc_decref(ctx, source);
}

}
