#pragma once
///@file
#include "bsd_api_expr.h"
#include "bsd_api_value.h"
#include "bsd/store/tests/bsd_api_store.hh"

#include <gtest/gtest.h>

namespace bsdC {

class bsd_api_expr_test : public bsd_api_store_test
{
protected:

    bsd_api_expr_test()
    {
        bsd_libexpr_init(ctx);
        state = bsd_state_create(nullptr, nullptr, store);
        value = bsd_alloc_value(nullptr, state);
    }
    ~bsd_api_expr_test()
    {
        bsd_gc_decref(nullptr, value);
        bsd_state_free(state);
    }

    EvalState * state;
    bsd_value * value;
};

}
