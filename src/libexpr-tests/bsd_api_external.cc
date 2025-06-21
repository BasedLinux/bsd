#include "bsd_api_store.h"
#include "bsd_api_store_internal.h"
#include "bsd_api_util.h"
#include "bsd_api_util_internal.h"
#include "bsd_api_expr.h"
#include "bsd_api_expr_internal.h"
#include "bsd_api_value.h"
#include "bsd_api_external.h"

#include "bsd/expr/tests/bsd_api_expr.hh"
#include "bsd/util/tests/string_callback.hh"

#include <gtest/gtest.h>

namespace bsdC {

class MyExternalValueDesc : public BsdCExternalValueDesc
{
public:
    MyExternalValueDesc(int x)
        : _x(x)
    {
        print = print_function;
        showType = show_type_function;
        typeOf = type_of_function;
    }

private:
    int _x;
    static void print_function(void * self, bsd_printer * printer) {}

    static void show_type_function(void * self, bsd_string_return * res) {}

    static void type_of_function(void * self, bsd_string_return * res)
    {
        MyExternalValueDesc * obj = static_cast<MyExternalValueDesc *>(self);

        std::string type_string = "bsd-external<MyExternalValueDesc( ";
        type_string += std::to_string(obj->_x);
        type_string += " )>";
        res->str = &*type_string.begin();
    }
};

TEST_F(bsd_api_expr_test, bsd_expr_eval_external)
{
    MyExternalValueDesc * external = new MyExternalValueDesc(42);
    ExternalValue * val = bsd_create_external_value(ctx, external, external);
    bsd_init_external(ctx, value, val);

    EvalState * stateResult = bsd_state_create(nullptr, nullptr, store);
    bsd_value * valueResult = bsd_alloc_value(nullptr, stateResult);

    EvalState * stateFn = bsd_state_create(nullptr, nullptr, store);
    bsd_value * valueFn = bsd_alloc_value(nullptr, stateFn);

    bsd_expr_eval_from_string(nullptr, state, "builtins.typeOf", ".", valueFn);

    ASSERT_EQ(NIX_TYPE_EXTERNAL, bsd_get_type(nullptr, value));

    bsd_value_call(ctx, state, valueFn, value, valueResult);

    std::string string_value;
    bsd_get_string(nullptr, valueResult, OBSERVE_STRING(string_value));
    ASSERT_STREQ("bsd-external<MyExternalValueDesc( 42 )>", string_value.c_str());

    bsd_state_free(stateResult);
    bsd_state_free(stateFn);
}

}
