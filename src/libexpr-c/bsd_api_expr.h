#ifndef NIX_API_EXPR_H
#define NIX_API_EXPR_H
/** @defgroup libexpr libexpr
 * @brief Bindings to the Bsd language evaluator
 *
 * See *[Embedding the Bsd Evaluator](@ref bsd_evaluator_example)* for an example.
 * @{
 */
/** @file
 * @brief Main entry for the libexpr C bindings
 */

#include "bsd_api_store.h"
#include "bsd_api_util.h"
#include <stddef.h>

#ifndef __has_c_attribute
#  define __has_c_attribute(x) 0
#endif

#if __has_c_attribute(deprecated)
#  define NIX_DEPRECATED(msg) [[deprecated(msg)]]
#else
#  define NIX_DEPRECATED(msg)
#endif

#ifdef __cplusplus
extern "C" {
#endif
// cffi start

// Type definitions
/**
 * @brief Builder for EvalState
 */
typedef struct bsd_eval_state_builder bsd_eval_state_builder;

/**
 * @brief Represents a state of the Bsd language evaluator.
 *
 * Multiple states can be created for multi-threaded
 * operation.
 * @struct EvalState
 * @see bsd_state_create
 */
typedef struct EvalState EvalState; // bsd::EvalState

/** @brief A Bsd language value, or thunk that may evaluate to a value.
 *
 * Values are the primary objects manipulated in the Bsd language.
 * They are considered to be immutable from a user's perspective, but the process of evaluating a value changes its
 * ValueType if it was a thunk. After a value has been evaluated, its ValueType does not change.
 *
 * Evaluation in this context refers to the process of evaluating a single value object, also called "forcing" the
 * value; see `bsd_value_force`.
 *
 * The evaluator manages its own memory, but your use of the C API must follow the reference counting rules.
 *
 * @see value_manip
 * @see bsd_value_incref, bsd_value_decref
 */
typedef struct bsd_value bsd_value;
NIX_DEPRECATED("use bsd_value instead") typedef bsd_value Value;

// Function prototypes
/**
 * @brief Initialize the Bsd language evaluator.
 *
 * This function must be called at least once,
 * at some point before constructing a EvalState for the first time.
 * This function can be called multiple times, and is idempotent.
 *
 * @param[out] context Optional, stores error information
 * @return NIX_OK if the initialization was successful, an error code otherwise.
 */
bsd_err bsd_libexpr_init(bsd_c_context * context);

/**
 * @brief Parses and evaluates a Bsd expression from a string.
 *
 * @param[out] context Optional, stores error information
 * @param[in] state The state of the evaluation.
 * @param[in] expr The Bsd expression to parse.
 * @param[in] path The file path to associate with the expression.
 * This is required for expressions that contain relative paths (such as `./.`) that are resolved relative to the given
 * directory.
 * @param[out] value The result of the evaluation. You must allocate this
 * yourself.
 * @return NIX_OK if the evaluation was successful, an error code otherwise.
 */
bsd_err bsd_expr_eval_from_string(
    bsd_c_context * context, EvalState * state, const char * expr, const char * path, bsd_value * value);

/**
 * @brief Calls a Bsd function with an argument.
 *
 * @param[out] context Optional, stores error information
 * @param[in] state The state of the evaluation.
 * @param[in] fn The Bsd function to call.
 * @param[in] arg The argument to pass to the function.
 * @param[out] value The result of the function call.
 * @return NIX_OK if the function call was successful, an error code otherwise.
 * @see bsd_init_apply() for a similar function that does not performs the call immediately, but stores it as a thunk.
 *      Note the different argument order.
 */
bsd_err bsd_value_call(bsd_c_context * context, EvalState * state, bsd_value * fn, bsd_value * arg, bsd_value * value);

/**
 * @brief Calls a Bsd function with multiple arguments.
 *
 * Technically these are functions that return functions. It is common for Bsd
 * functions to be curried, so this function is useful for calling them.
 *
 * @param[out] context Optional, stores error information
 * @param[in] state The state of the evaluation.
 * @param[in] fn The Bsd function to call.
 * @param[in] nargs The number of arguments.
 * @param[in] args The arguments to pass to the function.
 * @param[out] value The result of the function call.
 *
 * @see bsd_value_call     For the single argument primitive.
 * @see NIX_VALUE_CALL           For a macro that wraps this function for convenience.
 */
bsd_err bsd_value_call_multi(
    bsd_c_context * context, EvalState * state, bsd_value * fn, size_t nargs, bsd_value ** args, bsd_value * value);

/**
 * @brief Calls a Bsd function with multiple arguments.
 *
 * Technically these are functions that return functions. It is common for Bsd
 * functions to be curried, so this function is useful for calling them.
 *
 * @param[out] context Optional, stores error information
 * @param[in] state The state of the evaluation.
 * @param[out] value The result of the function call.
 * @param[in] fn The Bsd function to call.
 * @param[in] ... The arguments to pass to the function.
 *
 * @see bsd_value_call_multi
 */
#define NIX_VALUE_CALL(context, state, value, fn, ...)                      \
    do {                                                                    \
        bsd_value * args_array[] = {__VA_ARGS__};                           \
        size_t nargs = sizeof(args_array) / sizeof(args_array[0]);          \
        bsd_value_call_multi(context, state, fn, nargs, args_array, value); \
    } while (0)

/**
 * @brief Forces the evaluation of a Bsd value.
 *
 * The Bsd interpreter is lazy, and not-yet-evaluated values can be
 * of type NIX_TYPE_THUNK instead of their actual value.
 *
 * This function mutates such a `bsd_value`, so that, if successful, it has its final type.
 *
 * @param[out] context Optional, stores error information
 * @param[in] state The state of the evaluation.
 * @param[in,out] value The Bsd value to force.
 * @post value is not of type NIX_TYPE_THUNK
 * @return NIX_OK if the force operation was successful, an error code
 * otherwise.
 */
bsd_err bsd_value_force(bsd_c_context * context, EvalState * state, bsd_value * value);

/**
 * @brief Forces the deep evaluation of a Bsd value.
 *
 * Recursively calls bsd_value_force
 *
 * @see bsd_value_force
 * @warning Calling this function on a recursive data structure will cause a
 * stack overflow.
 * @param[out] context Optional, stores error information
 * @param[in] state The state of the evaluation.
 * @param[in,out] value The Bsd value to force.
 * @return NIX_OK if the deep force operation was successful, an error code
 * otherwise.
 */
bsd_err bsd_value_force_deep(bsd_c_context * context, EvalState * state, bsd_value * value);

/**
 * @brief Create a new bsd_eval_state_builder
 *
 * The settings are initialized to their default value.
 * Values can be sourced elsewhere with bsd_eval_state_builder_load.
 *
 * @param[out] context Optional, stores error information
 * @param[in] store The Bsd store to use.
 * @return A new bsd_eval_state_builder or NULL on failure.
 */
bsd_eval_state_builder * bsd_eval_state_builder_new(bsd_c_context * context, Store * store);

/**
 * @brief Read settings from the ambient environment
 *
 * Settings are sourced from environment variables and configuration files,
 * as documented in the Bsd manual.
 *
 * @param[out] context Optional, stores error information
 * @param[out] builder The builder to modify.
 * @return NIX_OK if successful, an error code otherwise.
 */
bsd_err bsd_eval_state_builder_load(bsd_c_context * context, bsd_eval_state_builder * builder);

/**
 * @brief Set the lookup path for `<...>` expressions
 *
 * @param[in] context Optional, stores error information
 * @param[in] builder The builder to modify.
 * @param[in] lookupPath Null-terminated array of strings corresponding to entries in NIX_PATH.
 */
bsd_err bsd_eval_state_builder_set_lookup_path(
    bsd_c_context * context, bsd_eval_state_builder * builder, const char ** lookupPath);

/**
 * @brief Create a new Bsd language evaluator state
 *
 * Remember to bsd_eval_state_builder_free after building the state.
 *
 * @param[out] context Optional, stores error information
 * @param[in] builder The builder to use and free
 * @return A new Bsd state or NULL on failure.
 * @see bsd_eval_state_builder_new, bsd_eval_state_builder_free
 */
EvalState * bsd_eval_state_build(bsd_c_context * context, bsd_eval_state_builder * builder);

/**
 * @brief Free a bsd_eval_state_builder
 *
 * Does not fail.
 *
 * @param[in] builder The builder to free.
 */
void bsd_eval_state_builder_free(bsd_eval_state_builder * builder);

/**
 * @brief Create a new Bsd language evaluator state
 *
 * For more control, use bsd_eval_state_builder
 *
 * @param[out] context Optional, stores error information
 * @param[in] lookupPath Null-terminated array of strings corresponding to entries in NIX_PATH.
 * @param[in] store The Bsd store to use.
 * @return A new Bsd state or NULL on failure.
 * @see bsd_state_builder_new
 */
EvalState * bsd_state_create(bsd_c_context * context, const char ** lookupPath, Store * store);

/**
 * @brief Frees a Bsd state.
 *
 * Does not fail.
 *
 * @param[in] state The state to free.
 */
void bsd_state_free(EvalState * state);

/** @addtogroup GC
 * @brief Reference counting and garbage collector operations
 *
 * The Bsd language evaluator uses a garbage collector. To ease C interop, we implement
 * a reference counting scheme, where objects will be deallocated
 * when there are no references from the Bsd side, and the reference count kept
 * by the C API reaches `0`.
 *
 * Functions returning a garbage-collected object will automatically increase
 * the refcount for you. You should make sure to call `bsd_gc_decref` when
 * you're done with a value returned by the evaluator.
 * @{
 */

// TODO: Deprecate bsd_gc_incref in favor of the type-specific reference counting functions?
//       e.g. bsd_value_incref.
//       It gives implementors more flexibility, and adds safety, so that generated
//       bindings can be used without fighting the host type system (where applicable).
/**
 * @brief Increment the garbage collector reference counter for the given object.
 *
 * The Bsd language evaluator C API keeps track of alive objects by reference counting.
 * When you're done with a refcounted pointer, call bsd_gc_decref().
 *
 * @param[out] context Optional, stores error information
 * @param[in] object The object to keep alive
 */
bsd_err bsd_gc_incref(bsd_c_context * context, const void * object);
/**
 * @brief Decrement the garbage collector reference counter for the given object
 *
 * We also provide typed `bsd_*_decref` functions, which are
 *   - safer to use
 *   - easier to integrate when deriving bindings
 *   - allow more flexibility
 *
 * @param[out] context Optional, stores error information
 * @param[in] object The object to stop referencing
 */
bsd_err bsd_gc_decref(bsd_c_context * context, const void * object);

/**
 * @brief Trigger the garbage collector manually
 *
 * You should not need to do this, but it can be useful for debugging.
 */
void bsd_gc_now();

/**
 * @brief Register a callback that gets called when the object is garbage
 * collected.
 * @note Objects can only have a single finalizer. This function overwrites existing values
 * silently.
 * @param[in] obj the object to watch
 * @param[in] cd the data to pass to the finalizer
 * @param[in] finalizer the callback function, called with obj and cd
 */
void bsd_gc_register_finalizer(void * obj, void * cd, void (*finalizer)(void * obj, void * cd));

/** @} */
// cffi end
#ifdef __cplusplus
}
#endif

/** @} */

#endif // NIX_API_EXPR_H
