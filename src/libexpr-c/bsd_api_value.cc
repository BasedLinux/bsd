#include "bsd/expr/attr-set.hh"
#include "bsd/util/configuration.hh"
#include "bsd/expr/eval.hh"
#include "bsd/store/globals.hh"
#include "bsd/store/path.hh"
#include "bsd/expr/primops.hh"
#include "bsd/expr/value.hh"

#include "bsd_api_expr.h"
#include "bsd_api_expr_internal.h"
#include "bsd_api_util.h"
#include "bsd_api_util_internal.h"
#include "bsd_api_store_internal.h"
#include "bsd_api_value.h"
#include "bsd/expr/value/context.hh"

// Internal helper functions to check [in] and [out] `Value *` parameters
static const bsd::Value & check_value_not_null(const bsd_value * value)
{
    if (!value) {
        throw std::runtime_error("bsd_value is null");
    }
    return *((const bsd::Value *) value);
}

static bsd::Value & check_value_not_null(bsd_value * value)
{
    if (!value) {
        throw std::runtime_error("bsd_value is null");
    }
    return value->value;
}

static const bsd::Value & check_value_in(const bsd_value * value)
{
    auto & v = check_value_not_null(value);
    if (!v.isValid()) {
        throw std::runtime_error("Uninitialized bsd_value");
    }
    return v;
}

static bsd::Value & check_value_in(bsd_value * value)
{
    auto & v = check_value_not_null(value);
    if (!v.isValid()) {
        throw std::runtime_error("Uninitialized bsd_value");
    }
    return v;
}

static bsd::Value & check_value_out(bsd_value * value)
{
    auto & v = check_value_not_null(value);
    if (v.isValid()) {
        throw std::runtime_error("bsd_value already initialized. Variables are immutable");
    }
    return v;
}

static inline bsd_value * as_bsd_value_ptr(bsd::Value * v)
{
    return reinterpret_cast<bsd_value *>(v);
}

/**
 * Helper function to convert calls from bsd into C API.
 *
 * Deals with errors and converts arguments from C++ into C types.
 */
static void bsd_c_primop_wrapper(
    PrimOpFun f, void * userdata, bsd::EvalState & state, const bsd::PosIdx pos, bsd::Value ** args, bsd::Value & v)
{
    bsd_c_context ctx;

    // v currently has a thunk, but the C API initializers require an uninitialized value.
    //
    // We can't destroy the thunk, because that makes it impossible to retry,
    // which is needed for tryEval and for evaluation drivers that evaluate more
    // than one value (e.g. an attrset with two derivations, both of which
    // reference v).
    //
    // Instead we create a temporary value, and then assign the result to v.
    // This does not give the primop definition access to the thunk, but that's
    // ok because we don't see a need for this yet (e.g. inspecting thunks,
    // or maybe something to make blackholes work better; we don't know).
    bsd::Value vTmp;

    f(userdata, &ctx, (EvalState *) &state, (bsd_value **) args, (bsd_value *) &vTmp);

    if (ctx.last_err_code != NIX_OK) {
        /* TODO: Throw different errors depending on the error code */
        state.error<bsd::EvalError>("Error from custom function: %s", *ctx.last_err).atPos(pos).debugThrow();
    }

    if (!vTmp.isValid()) {
        state.error<bsd::EvalError>("Implementation error in custom function: return value was not initialized")
            .atPos(pos)
            .debugThrow();
    }

    if (vTmp.type() == bsd::nThunk) {
        // We might allow this in the future if it makes sense for the evaluator
        // e.g. implementing tail recursion by returning a thunk to the next
        // "iteration". Until then, this is most likely a mistake or misunderstanding.
        state.error<bsd::EvalError>("Implementation error in custom function: return value must not be a thunk")
            .atPos(pos)
            .debugThrow();
    }

    v = vTmp;
}

PrimOp * bsd_alloc_primop(
    bsd_c_context * context,
    PrimOpFun fun,
    int arity,
    const char * name,
    const char ** args,
    const char * doc,
    void * user_data)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        using namespace std::placeholders;
        auto p = new
#if NIX_USE_BOEHMGC
            (GC)
#endif
                bsd::PrimOp{
                    .name = name,
                    .args = {},
                    .arity = (size_t) arity,
                    .doc = doc,
                    .fun = std::bind(bsd_c_primop_wrapper, fun, user_data, _1, _2, _3, _4)};
        if (args)
            for (size_t i = 0; args[i]; i++)
                p->args.emplace_back(*args);
        bsd_gc_incref(nullptr, p);
        return (PrimOp *) p;
    }
    NIXC_CATCH_ERRS_NULL
}

bsd_err bsd_register_primop(bsd_c_context * context, PrimOp * primOp)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        bsd::RegisterPrimOp r(std::move(*((bsd::PrimOp *) primOp)));
    }
    NIXC_CATCH_ERRS
}

bsd_value * bsd_alloc_value(bsd_c_context * context, EvalState * state)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        bsd_value * res = as_bsd_value_ptr(state->state.allocValue());
        bsd_gc_incref(nullptr, res);
        return res;
    }
    NIXC_CATCH_ERRS_NULL
}

ValueType bsd_get_type(bsd_c_context * context, const bsd_value * value)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_in(value);
        using namespace bsd;
        switch (v.type()) {
        case nThunk:
            return NIX_TYPE_THUNK;
        case nInt:
            return NIX_TYPE_INT;
        case nFloat:
            return NIX_TYPE_FLOAT;
        case nBool:
            return NIX_TYPE_BOOL;
        case nString:
            return NIX_TYPE_STRING;
        case nPath:
            return NIX_TYPE_PATH;
        case nNull:
            return NIX_TYPE_NULL;
        case nAttrs:
            return NIX_TYPE_ATTRS;
        case nList:
            return NIX_TYPE_LIST;
        case nFunction:
            return NIX_TYPE_FUNCTION;
        case nExternal:
            return NIX_TYPE_EXTERNAL;
        }
        return NIX_TYPE_NULL;
    }
    NIXC_CATCH_ERRS_RES(NIX_TYPE_NULL);
}

const char * bsd_get_typename(bsd_c_context * context, const bsd_value * value)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_in(value);
        auto s = bsd::showType(v);
        return strdup(s.c_str());
    }
    NIXC_CATCH_ERRS_NULL
}

bool bsd_get_bool(bsd_c_context * context, const bsd_value * value)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_in(value);
        assert(v.type() == bsd::nBool);
        return v.boolean();
    }
    NIXC_CATCH_ERRS_RES(false);
}

bsd_err
bsd_get_string(bsd_c_context * context, const bsd_value * value, bsd_get_string_callback callback, void * user_data)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_in(value);
        assert(v.type() == bsd::nString);
        call_bsd_get_string_callback(v.c_str(), callback, user_data);
    }
    NIXC_CATCH_ERRS
}

const char * bsd_get_path_string(bsd_c_context * context, const bsd_value * value)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_in(value);
        assert(v.type() == bsd::nPath);
        // NOTE (from @yorickvP)
        // v._path.path should work but may not be how Eelco intended it.
        // Long-term this function should be rewritten to copy some data into a
        // user-allocated string.
        // We could use v.path().to_string().c_str(), but I'm concerned this
        // crashes. Looks like .path() allocates a CanonPath with a copy of the
        // string, then it gets the underlying data from that.
        return v.pathStr();
    }
    NIXC_CATCH_ERRS_NULL
}

unsigned int bsd_get_list_size(bsd_c_context * context, const bsd_value * value)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_in(value);
        assert(v.type() == bsd::nList);
        return v.listSize();
    }
    NIXC_CATCH_ERRS_RES(0);
}

unsigned int bsd_get_attrs_size(bsd_c_context * context, const bsd_value * value)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_in(value);
        assert(v.type() == bsd::nAttrs);
        return v.attrs()->size();
    }
    NIXC_CATCH_ERRS_RES(0);
}

double bsd_get_float(bsd_c_context * context, const bsd_value * value)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_in(value);
        assert(v.type() == bsd::nFloat);
        return v.fpoint();
    }
    NIXC_CATCH_ERRS_RES(0.0);
}

int64_t bsd_get_int(bsd_c_context * context, const bsd_value * value)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_in(value);
        assert(v.type() == bsd::nInt);
        return v.integer().value;
    }
    NIXC_CATCH_ERRS_RES(0);
}

ExternalValue * bsd_get_external(bsd_c_context * context, bsd_value * value)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_out(value);
        assert(v.type() == bsd::nExternal);
        return (ExternalValue *) v.external();
    }
    NIXC_CATCH_ERRS_NULL;
}

bsd_value * bsd_get_list_byidx(bsd_c_context * context, const bsd_value * value, EvalState * state, unsigned int ix)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_in(value);
        assert(v.type() == bsd::nList);
        auto * p = v.listElems()[ix];
        bsd_gc_incref(nullptr, p);
        if (p != nullptr)
            state->state.forceValue(*p, bsd::noPos);
        return as_bsd_value_ptr(p);
    }
    NIXC_CATCH_ERRS_NULL
}

bsd_value * bsd_get_attr_byname(bsd_c_context * context, const bsd_value * value, EvalState * state, const char * name)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_in(value);
        assert(v.type() == bsd::nAttrs);
        bsd::Symbol s = state->state.symbols.create(name);
        auto attr = v.attrs()->get(s);
        if (attr) {
            bsd_gc_incref(nullptr, attr->value);
            state->state.forceValue(*attr->value, bsd::noPos);
            return as_bsd_value_ptr(attr->value);
        }
        bsd_set_err_msg(context, NIX_ERR_KEY, "missing attribute");
        return nullptr;
    }
    NIXC_CATCH_ERRS_NULL
}

bool bsd_has_attr_byname(bsd_c_context * context, const bsd_value * value, EvalState * state, const char * name)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_in(value);
        assert(v.type() == bsd::nAttrs);
        bsd::Symbol s = state->state.symbols.create(name);
        auto attr = v.attrs()->get(s);
        if (attr)
            return true;
        return false;
    }
    NIXC_CATCH_ERRS_RES(false);
}

bsd_value * bsd_get_attr_byidx(
    bsd_c_context * context, const bsd_value * value, EvalState * state, unsigned int i, const char ** name)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_in(value);
        const bsd::Attr & a = (*v.attrs())[i];
        *name = state->state.symbols[a.name].c_str();
        bsd_gc_incref(nullptr, a.value);
        state->state.forceValue(*a.value, bsd::noPos);
        return as_bsd_value_ptr(a.value);
    }
    NIXC_CATCH_ERRS_NULL
}

const char *
bsd_get_attr_name_byidx(bsd_c_context * context, const bsd_value * value, EvalState * state, unsigned int i)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_in(value);
        const bsd::Attr & a = (*v.attrs())[i];
        return state->state.symbols[a.name].c_str();
    }
    NIXC_CATCH_ERRS_NULL
}

bsd_err bsd_init_bool(bsd_c_context * context, bsd_value * value, bool b)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_out(value);
        v.mkBool(b);
    }
    NIXC_CATCH_ERRS
}

// todo string context
bsd_err bsd_init_string(bsd_c_context * context, bsd_value * value, const char * str)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_out(value);
        v.mkString(std::string_view(str));
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_init_path_string(bsd_c_context * context, EvalState * s, bsd_value * value, const char * str)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_out(value);
        v.mkPath(s->state.rootPath(bsd::CanonPath(str)));
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_init_float(bsd_c_context * context, bsd_value * value, double d)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_out(value);
        v.mkFloat(d);
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_init_int(bsd_c_context * context, bsd_value * value, int64_t i)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_out(value);
        v.mkInt(i);
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_init_null(bsd_c_context * context, bsd_value * value)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_out(value);
        v.mkNull();
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_init_apply(bsd_c_context * context, bsd_value * value, bsd_value * fn, bsd_value * arg)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_not_null(value);
        auto & f = check_value_not_null(fn);
        auto & a = check_value_not_null(arg);
        v.mkApp(&f, &a);
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_init_external(bsd_c_context * context, bsd_value * value, ExternalValue * val)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_out(value);
        auto r = (bsd::ExternalValueBase *) val;
        v.mkExternal(r);
    }
    NIXC_CATCH_ERRS
}

ListBuilder * bsd_make_list_builder(bsd_c_context * context, EvalState * state, size_t capacity)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto builder = state->state.buildList(capacity);
        return new
#if NIX_USE_BOEHMGC
            (NoGC)
#endif
                ListBuilder{std::move(builder)};
    }
    NIXC_CATCH_ERRS_NULL
}

bsd_err
bsd_list_builder_insert(bsd_c_context * context, ListBuilder * list_builder, unsigned int index, bsd_value * value)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & e = check_value_not_null(value);
        list_builder->builder[index] = &e;
    }
    NIXC_CATCH_ERRS
}

void bsd_list_builder_free(ListBuilder * list_builder)
{
#if NIX_USE_BOEHMGC
    GC_FREE(list_builder);
#else
    delete list_builder;
#endif
}

bsd_err bsd_make_list(bsd_c_context * context, ListBuilder * list_builder, bsd_value * value)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_out(value);
        v.mkList(list_builder->builder);
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_init_primop(bsd_c_context * context, bsd_value * value, PrimOp * p)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_out(value);
        v.mkPrimOp((bsd::PrimOp *) p);
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_copy_value(bsd_c_context * context, bsd_value * value, const bsd_value * source)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_out(value);
        auto & s = check_value_in(source);
        v = s;
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_make_attrs(bsd_c_context * context, bsd_value * value, BindingsBuilder * b)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_out(value);
        v.mkAttrs(b->builder);
    }
    NIXC_CATCH_ERRS
}

BindingsBuilder * bsd_make_bindings_builder(bsd_c_context * context, EvalState * state, size_t capacity)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto bb = state->state.buildBindings(capacity);
        return new
#if NIX_USE_BOEHMGC
            (NoGC)
#endif
                BindingsBuilder{std::move(bb)};
    }
    NIXC_CATCH_ERRS_NULL
}

bsd_err bsd_bindings_builder_insert(bsd_c_context * context, BindingsBuilder * bb, const char * name, bsd_value * value)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_not_null(value);
        bsd::Symbol s = bb->builder.state.symbols.create(name);
        bb->builder.insert(s, &v);
    }
    NIXC_CATCH_ERRS
}

void bsd_bindings_builder_free(BindingsBuilder * bb)
{
#if NIX_USE_BOEHMGC
    GC_FREE((bsd::BindingsBuilder *) bb);
#else
    delete (bsd::BindingsBuilder *) bb;
#endif
}

bsd_realised_string * bsd_string_realise(bsd_c_context * context, EvalState * state, bsd_value * value, bool isIFD)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto & v = check_value_in(value);
        bsd::StorePathSet storePaths;
        auto s = state->state.realiseString(v, &storePaths, isIFD);

        // Convert to the C API StorePath type and convert to vector for index-based access
        std::vector<StorePath> vec;
        for (auto & sp : storePaths) {
            vec.push_back(StorePath{sp});
        }

        return new bsd_realised_string{.str = s, .storePaths = vec};
    }
    NIXC_CATCH_ERRS_NULL
}

void bsd_realised_string_free(bsd_realised_string * s)
{
    delete s;
}

size_t bsd_realised_string_get_buffer_size(bsd_realised_string * s)
{
    return s->str.size();
}

const char * bsd_realised_string_get_buffer_start(bsd_realised_string * s)
{
    return s->str.data();
}

size_t bsd_realised_string_get_store_path_count(bsd_realised_string * s)
{
    return s->storePaths.size();
}

const StorePath * bsd_realised_string_get_store_path(bsd_realised_string * s, size_t i)
{
    return &s->storePaths[i];
}
