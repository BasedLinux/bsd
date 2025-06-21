#include <cstring>
#include <stdexcept>
#include <string>

#include "bsd/expr/eval.hh"
#include "bsd/expr/eval-gc.hh"
#include "bsd/store/globals.hh"
#include "bsd/expr/eval-settings.hh"
#include "bsd/util/ref.hh"

#include "bsd_api_expr.h"
#include "bsd_api_expr_internal.h"
#include "bsd_api_store.h"
#include "bsd_api_store_internal.h"
#include "bsd_api_util.h"
#include "bsd_api_util_internal.h"

#if NIX_USE_BOEHMGC
#  include <mutex>
#endif

/**
 * @brief Allocate and initialize using self-reference
 *
 * This allows a brace initializer to reference the object being constructed.
 *
 * @warning Use with care, as the pointer points to an object that is not fully constructed yet.
 *
 * @tparam T Type to allocate
 * @tparam F A function type for `init`, taking a T* and returning the initializer for T
 * @param init Function that takes a T* and returns the initializer for T
 * @return Pointer to allocated and initialized object
 */
template <typename T, typename F>
static T * unsafe_new_with_self(F && init)
{
    // Allocate
    void * p = ::operator new(
        sizeof(T),
        static_cast<std::align_val_t>(alignof(T)));
    // Initialize with placement new
    return new (p) T(init(static_cast<T *>(p)));
}

bsd_err bsd_libexpr_init(bsd_c_context * context)
{
    if (context)
        context->last_err_code = NIX_OK;
    {
        auto ret = bsd_libutil_init(context);
        if (ret != NIX_OK)
            return ret;
    }
    {
        auto ret = bsd_libstore_init(context);
        if (ret != NIX_OK)
            return ret;
    }
    try {
        bsd::initGC();
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_expr_eval_from_string(
    bsd_c_context * context, EvalState * state, const char * expr, const char * path, bsd_value * value)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        bsd::Expr * parsedExpr = state->state.parseExprFromString(expr, state->state.rootPath(bsd::CanonPath(path)));
        state->state.eval(parsedExpr, value->value);
        state->state.forceValue(value->value, bsd::noPos);
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_value_call(bsd_c_context * context, EvalState * state, Value * fn, bsd_value * arg, bsd_value * value)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        state->state.callFunction(fn->value, arg->value, value->value, bsd::noPos);
        state->state.forceValue(value->value, bsd::noPos);
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_value_call_multi(bsd_c_context * context, EvalState * state, bsd_value * fn, size_t nargs, bsd_value ** args, bsd_value * value)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        state->state.callFunction(fn->value, {(bsd::Value * *) args, nargs}, value->value, bsd::noPos);
        state->state.forceValue(value->value, bsd::noPos);
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_value_force(bsd_c_context * context, EvalState * state, bsd_value * value)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        state->state.forceValue(value->value, bsd::noPos);
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_value_force_deep(bsd_c_context * context, EvalState * state, bsd_value * value)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        state->state.forceValueDeep(value->value);
    }
    NIXC_CATCH_ERRS
}

bsd_eval_state_builder * bsd_eval_state_builder_new(bsd_c_context * context, Store * store)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        return unsafe_new_with_self<bsd_eval_state_builder>([&](auto * self) {
            return bsd_eval_state_builder{
                .store = bsd::ref<bsd::Store>(store->ptr),
                .settings = bsd::EvalSettings{/* &bool */ self->readOnlyMode},
                .fetchSettings = bsd::fetchers::Settings{},
                .readOnlyMode = true,
            };
        });
    }
    NIXC_CATCH_ERRS_NULL
}

void bsd_eval_state_builder_free(bsd_eval_state_builder * builder)
{
    delete builder;
}

bsd_err bsd_eval_state_builder_load(bsd_c_context * context, bsd_eval_state_builder * builder)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        // TODO: load in one go?
        builder->settings.readOnlyMode = bsd::settings.readOnlyMode;
        loadConfFile(builder->settings);
        loadConfFile(builder->fetchSettings);
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_eval_state_builder_set_lookup_path(bsd_c_context * context, bsd_eval_state_builder * builder, const char ** lookupPath_c)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        bsd::Strings lookupPath;
        if (lookupPath_c != nullptr)
            for (size_t i = 0; lookupPath_c[i] != nullptr; i++)
                lookupPath.push_back(lookupPath_c[i]);
        builder->lookupPath = bsd::LookupPath::parse(lookupPath);
    }
    NIXC_CATCH_ERRS
}

EvalState * bsd_eval_state_build(bsd_c_context * context, bsd_eval_state_builder * builder)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        return unsafe_new_with_self<EvalState>([&](auto * self) {
            return EvalState{
                .fetchSettings = std::move(builder->fetchSettings),
                .settings = std::move(builder->settings),
                .state = bsd::EvalState(
                    builder->lookupPath,
                    builder->store,
                    self->fetchSettings,
                    self->settings),
            };
        });
    }
    NIXC_CATCH_ERRS_NULL
}

EvalState * bsd_state_create(bsd_c_context * context, const char ** lookupPath_c, Store * store)
{
    auto builder = bsd_eval_state_builder_new(context, store);
    if (builder == nullptr)
        return nullptr;

    if (bsd_eval_state_builder_load(context, builder) != NIX_OK)
        return nullptr;

    if (bsd_eval_state_builder_set_lookup_path(context, builder, lookupPath_c)
            != NIX_OK)
        return nullptr;

    auto *state = bsd_eval_state_build(context, builder);
    bsd_eval_state_builder_free(builder);
    return state;
}

void bsd_state_free(EvalState * state)
{
    delete state;
}

#if NIX_USE_BOEHMGC
std::unordered_map<
    const void *,
    unsigned int,
    std::hash<const void *>,
    std::equal_to<const void *>,
    traceable_allocator<std::pair<const void * const, unsigned int>>>
    bsd_refcounts;

std::mutex bsd_refcount_lock;

bsd_err bsd_gc_incref(bsd_c_context * context, const void * p)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        std::scoped_lock lock(bsd_refcount_lock);
        auto f = bsd_refcounts.find(p);
        if (f != bsd_refcounts.end()) {
            f->second++;
        } else {
            bsd_refcounts[p] = 1;
        }
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_gc_decref(bsd_c_context * context, const void * p)
{

    if (context)
        context->last_err_code = NIX_OK;
    try {
        std::scoped_lock lock(bsd_refcount_lock);
        auto f = bsd_refcounts.find(p);
        if (f != bsd_refcounts.end()) {
            if (--f->second == 0)
                bsd_refcounts.erase(f);
        } else
            throw std::runtime_error("bsd_gc_decref: object was not referenced");
    }
    NIXC_CATCH_ERRS
}

void bsd_gc_now()
{
    GC_gcollect();
}

#else
bsd_err bsd_gc_incref(bsd_c_context * context, const void *)
{
    if (context)
        context->last_err_code = NIX_OK;
    return NIX_OK;
}
bsd_err bsd_gc_decref(bsd_c_context * context, const void *)
{
    if (context)
        context->last_err_code = NIX_OK;
    return NIX_OK;
}
void bsd_gc_now() {}
#endif

bsd_err bsd_value_incref(bsd_c_context * context, bsd_value *x)
{
    return bsd_gc_incref(context, (const void *) x);
}
bsd_err bsd_value_decref(bsd_c_context * context, bsd_value *x)
{
    return bsd_gc_decref(context, (const void *) x);
}

void bsd_gc_register_finalizer(void * obj, void * cd, void (*finalizer)(void * obj, void * cd))
{
#if NIX_USE_BOEHMGC
    GC_REGISTER_FINALIZER(obj, finalizer, cd, 0, 0);
#endif
}
