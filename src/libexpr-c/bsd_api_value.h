#ifndef NIX_API_VALUE_H
#define NIX_API_VALUE_H

/** @addtogroup libexpr
 * @{
 */
/** @file
 * @brief libexpr C bindings dealing with values
 */

#include "bsd_api_util.h"
#include "bsd_api_store.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
// cffi start

// Type definitions
typedef enum {
    NIX_TYPE_THUNK,
    NIX_TYPE_INT,
    NIX_TYPE_FLOAT,
    NIX_TYPE_BOOL,
    NIX_TYPE_STRING,
    NIX_TYPE_PATH,
    NIX_TYPE_NULL,
    NIX_TYPE_ATTRS,
    NIX_TYPE_LIST,
    NIX_TYPE_FUNCTION,
    NIX_TYPE_EXTERNAL
} ValueType;

// forward declarations
typedef struct bsd_value bsd_value;
typedef struct EvalState EvalState;

[[deprecated("use bsd_value instead")]] typedef bsd_value Value;

// type defs
/** @brief Stores an under-construction set of bindings
 * @ingroup value_manip
 *
 * Do not reuse.
 * @see bsd_make_bindings_builder, bsd_bindings_builder_free, bsd_make_attrs
 * @see bsd_bindings_builder_insert
 */
typedef struct BindingsBuilder BindingsBuilder;

/** @brief Stores an under-construction list
 * @ingroup value_manip
 *
 * Do not reuse.
 * @see bsd_make_list_builder, bsd_list_builder_free, bsd_make_list
 * @see bsd_list_builder_insert
 */
typedef struct ListBuilder ListBuilder;

/** @brief PrimOp function
 * @ingroup primops
 *
 * Owned by the GC
 * @see bsd_alloc_primop, bsd_init_primop
 */
typedef struct PrimOp PrimOp;
/** @brief External Value
 * @ingroup Externals
 *
 * Owned by the GC
 */
typedef struct ExternalValue ExternalValue;

/** @brief String without placeholders, and realised store paths
 */
typedef struct bsd_realised_string bsd_realised_string;

/** @defgroup primops Adding primops
 * @{
 */
/** @brief Function pointer for primops
 *
 * When you want to return an error, call bsd_set_err_msg(context, NIX_ERR_UNKNOWN, "your error message here").
 *
 * @param[in] user_data Arbitrary data that was initially supplied to bsd_alloc_primop
 * @param[out] context Stores error information.
 * @param[in] state Evaluator state
 * @param[in] args list of arguments. Note that these can be thunks and should be forced using bsd_value_force before
 * use.
 * @param[out] ret return value
 * @see bsd_alloc_primop, bsd_init_primop
 */
typedef void (*PrimOpFun)(
    void * user_data, bsd_c_context * context, EvalState * state, bsd_value ** args, bsd_value * ret);

/** @brief Allocate a PrimOp
 *
 * Owned by the garbage collector.
 * Use bsd_gc_decref() when you're done with the returned PrimOp.
 *
 * @param[out] context Optional, stores error information
 * @param[in] fun callback
 * @param[in] arity expected number of function arguments
 * @param[in] name function name
 * @param[in] args array of argument names, NULL-terminated
 * @param[in] doc optional, documentation for this primop
 * @param[in] user_data optional, arbitrary data, passed to the callback when it's called
 * @return primop, or null in case of errors
 * @see bsd_init_primop
 */
PrimOp * bsd_alloc_primop(
    bsd_c_context * context,
    PrimOpFun fun,
    int arity,
    const char * name,
    const char ** args,
    const char * doc,
    void * user_data);

/** @brief add a primop to the `builtins` attribute set
 *
 * Only applies to States created after this call.
 *
 * Moves your PrimOp content into the global evaluator
 * registry, meaning your input PrimOp pointer is no longer usable.
 * You are free to remove your references to it,
 * after which it will be garbage collected.
 *
 * @param[out] context Optional, stores error information
 * @return primop, or null in case of errors
 *
 */
bsd_err bsd_register_primop(bsd_c_context * context, PrimOp * primOp);
/** @} */

// Function prototypes

/** @brief Allocate a Bsd value
 *
 * Owned by the GC. Use bsd_gc_decref() when you're done with the pointer
 * @param[out] context Optional, stores error information
 * @param[in] state bsd evaluator state
 * @return value, or null in case of errors
 *
 */
bsd_value * bsd_alloc_value(bsd_c_context * context, EvalState * state);

/**
 * @brief Increment the garbage collector reference counter for the given `bsd_value`.
 *
 * The Bsd language evaluator C API keeps track of alive objects by reference counting.
 * When you're done with a refcounted pointer, call bsd_value_decref().
 *
 * @param[out] context Optional, stores error information
 * @param[in] value The object to keep alive
 */
bsd_err bsd_value_incref(bsd_c_context * context, bsd_value * value);

/**
 * @brief Decrement the garbage collector reference counter for the given object
 *
 * @param[out] context Optional, stores error information
 * @param[in] value The object to stop referencing
 */
bsd_err bsd_value_decref(bsd_c_context * context, bsd_value * value);

/** @addtogroup value_manip Manipulating values
 * @brief Functions to inspect and change Bsd language values, represented by bsd_value.
 * @{
 */
/** @anchor getters
 * @name Getters
 */
/**@{*/
/** @brief Get value type
 * @param[out] context Optional, stores error information
 * @param[in] value Bsd value to inspect
 * @return type of bsd value
 */
ValueType bsd_get_type(bsd_c_context * context, const bsd_value * value);

/** @brief Get type name of value as defined in the evaluator
 * @param[out] context Optional, stores error information
 * @param[in] value Bsd value to inspect
 * @return type name, owned string
 * @todo way to free the result
 */
const char * bsd_get_typename(bsd_c_context * context, const bsd_value * value);

/** @brief Get boolean value
 * @param[out] context Optional, stores error information
 * @param[in] value Bsd value to inspect
 * @return true or false, error info via context
 */
bool bsd_get_bool(bsd_c_context * context, const bsd_value * value);

/** @brief Get the raw string
 *
 * This may contain placeholders.
 *
 * @param[out] context Optional, stores error information
 * @param[in] value Bsd value to inspect
 * @param[in] callback Called with the string value.
 * @param[in] user_data optional, arbitrary data, passed to the callback when it's called.
 * @return string
 * @return error code, NIX_OK on success.
 */
bsd_err
bsd_get_string(bsd_c_context * context, const bsd_value * value, bsd_get_string_callback callback, void * user_data);

/** @brief Get path as string
 * @param[out] context Optional, stores error information
 * @param[in] value Bsd value to inspect
 * @return string, if the type is NIX_TYPE_PATH
 * @return NULL in case of error.
 */
const char * bsd_get_path_string(bsd_c_context * context, const bsd_value * value);

/** @brief Get the length of a list
 * @param[out] context Optional, stores error information
 * @param[in] value Bsd value to inspect
 * @return length of list, error info via context
 */
unsigned int bsd_get_list_size(bsd_c_context * context, const bsd_value * value);

/** @brief Get the element count of an attrset
 * @param[out] context Optional, stores error information
 * @param[in] value Bsd value to inspect
 * @return attrset element count, error info via context
 */
unsigned int bsd_get_attrs_size(bsd_c_context * context, const bsd_value * value);

/** @brief Get float value in 64 bits
 * @param[out] context Optional, stores error information
 * @param[in] value Bsd value to inspect
 * @return float contents, error info via context
 */
double bsd_get_float(bsd_c_context * context, const bsd_value * value);

/** @brief Get int value
 * @param[out] context Optional, stores error information
 * @param[in] value Bsd value to inspect
 * @return int contents, error info via context
 */
int64_t bsd_get_int(bsd_c_context * context, const bsd_value * value);

/** @brief Get external reference
 * @param[out] context Optional, stores error information
 * @param[in] value Bsd value to inspect
 * @return reference to external, NULL in case of error
 */
ExternalValue * bsd_get_external(bsd_c_context * context, bsd_value * value);

/** @brief Get the ix'th element of a list
 *
 * Owned by the GC. Use bsd_gc_decref when you're done with the pointer
 * @param[out] context Optional, stores error information
 * @param[in] value Bsd value to inspect
 * @param[in] state bsd evaluator state
 * @param[in] ix list element to get
 * @return value, NULL in case of errors
 */
bsd_value * bsd_get_list_byidx(bsd_c_context * context, const bsd_value * value, EvalState * state, unsigned int ix);

/** @brief Get an attr by name
 *
 * Owned by the GC. Use bsd_gc_decref when you're done with the pointer
 * @param[out] context Optional, stores error information
 * @param[in] value Bsd value to inspect
 * @param[in] state bsd evaluator state
 * @param[in] name attribute name
 * @return value, NULL in case of errors
 */
bsd_value * bsd_get_attr_byname(bsd_c_context * context, const bsd_value * value, EvalState * state, const char * name);

/** @brief Check if an attribute name exists on a value
 * @param[out] context Optional, stores error information
 * @param[in] value Bsd value to inspect
 * @param[in] state bsd evaluator state
 * @param[in] name attribute name
 * @return value, error info via context
 */
bool bsd_has_attr_byname(bsd_c_context * context, const bsd_value * value, EvalState * state, const char * name);

/** @brief Get an attribute by index in the sorted bindings
 *
 * Also gives you the name.
 *
 * Owned by the GC. Use bsd_gc_decref when you're done with the pointer
 * @param[out] context Optional, stores error information
 * @param[in] value Bsd value to inspect
 * @param[in] state bsd evaluator state
 * @param[in] i attribute index
 * @param[out] name will store a pointer to the attribute name
 * @return value, NULL in case of errors
 */
bsd_value * bsd_get_attr_byidx(
    bsd_c_context * context, const bsd_value * value, EvalState * state, unsigned int i, const char ** name);

/** @brief Get an attribute name by index in the sorted bindings
 *
 * Useful when you want the name but want to avoid evaluation.
 *
 * Owned by the bsd EvalState
 * @param[out] context Optional, stores error information
 * @param[in] value Bsd value to inspect
 * @param[in] state bsd evaluator state
 * @param[in] i attribute index
 * @return name, NULL in case of errors
 */
const char *
bsd_get_attr_name_byidx(bsd_c_context * context, const bsd_value * value, EvalState * state, unsigned int i);

/**@}*/
/** @name Initializers
 *
 * Values are typically "returned" by initializing already allocated memory that serves as the return value.
 * For this reason, the construction of values is not tied their allocation.
 * Bsd is a language with immutable values. Respect this property by only initializing Values once; and only initialize
 * Values that are meant to be initialized by you. Failing to adhere to these rules may lead to undefined behavior.
 */
/**@{*/
/** @brief Set boolean value
 * @param[out] context Optional, stores error information
 * @param[out] value Bsd value to modify
 * @param[in] b the boolean value
 * @return error code, NIX_OK on success.
 */
bsd_err bsd_init_bool(bsd_c_context * context, bsd_value * value, bool b);

/** @brief Set a string
 * @param[out] context Optional, stores error information
 * @param[out] value Bsd value to modify
 * @param[in] str the string, copied
 * @return error code, NIX_OK on success.
 */
bsd_err bsd_init_string(bsd_c_context * context, bsd_value * value, const char * str);

/** @brief Set a path
 * @param[out] context Optional, stores error information
 * @param[out] value Bsd value to modify
 * @param[in] str the path string, copied
 * @return error code, NIX_OK on success.
 */
bsd_err bsd_init_path_string(bsd_c_context * context, EvalState * s, bsd_value * value, const char * str);

/** @brief Set a float
 * @param[out] context Optional, stores error information
 * @param[out] value Bsd value to modify
 * @param[in] d the float, 64-bits
 * @return error code, NIX_OK on success.
 */
bsd_err bsd_init_float(bsd_c_context * context, bsd_value * value, double d);

/** @brief Set an int
 * @param[out] context Optional, stores error information
 * @param[out] value Bsd value to modify
 * @param[in] i the int
 * @return error code, NIX_OK on success.
 */

bsd_err bsd_init_int(bsd_c_context * context, bsd_value * value, int64_t i);
/** @brief Set null
 * @param[out] context Optional, stores error information
 * @param[out] value Bsd value to modify
 * @return error code, NIX_OK on success.
 */
bsd_err bsd_init_null(bsd_c_context * context, bsd_value * value);

/** @brief Set the value to a thunk that will perform a function application when needed.
 *
 * Thunks may be put into attribute sets and lists to perform some computation lazily; on demand.
 * However, note that in some places, a thunk must not be returned, such as in the return value of a PrimOp.
 * In such cases, you may use bsd_value_call() instead (but note the different argument order).
 *
 * @param[out] context Optional, stores error information
 * @param[out] value Bsd value to modify
 * @param[in] fn function to call
 * @param[in] arg argument to pass
 * @return error code, NIX_OK on successful initialization.
 * @see bsd_value_call() for a similar function that performs the call immediately and only stores the return value.
 *      Note the different argument order.
 */
bsd_err bsd_init_apply(bsd_c_context * context, bsd_value * value, bsd_value * fn, bsd_value * arg);

/** @brief Set an external value
 * @param[out] context Optional, stores error information
 * @param[out] value Bsd value to modify
 * @param[in] val the external value to set. Will be GC-referenced by the value.
 * @return error code, NIX_OK on success.
 */
bsd_err bsd_init_external(bsd_c_context * context, bsd_value * value, ExternalValue * val);

/** @brief Create a list from a list builder
 * @param[out] context Optional, stores error information
 * @param[in] list_builder list builder to use. Make sure to unref this afterwards.
 * @param[out] value Bsd value to modify
 * @return error code, NIX_OK on success.
 */
bsd_err bsd_make_list(bsd_c_context * context, ListBuilder * list_builder, bsd_value * value);

/** @brief Create a list builder
 * @param[out] context Optional, stores error information
 * @param[in] state bsd evaluator state
 * @param[in] capacity how many bindings you'll add. Don't exceed.
 * @return owned reference to a list builder. Make sure to unref when you're done.
 */
ListBuilder * bsd_make_list_builder(bsd_c_context * context, EvalState * state, size_t capacity);

/** @brief Insert bindings into a builder
 * @param[out] context Optional, stores error information
 * @param[in] list_builder ListBuilder to insert into
 * @param[in] index index to manipulate
 * @param[in] value value to insert
 * @return error code, NIX_OK on success.
 */
bsd_err
bsd_list_builder_insert(bsd_c_context * context, ListBuilder * list_builder, unsigned int index, bsd_value * value);

/** @brief Free a list builder
 *
 * Does not fail.
 * @param[in] list_builder The builder to free.
 */
void bsd_list_builder_free(ListBuilder * list_builder);

/** @brief Create an attribute set from a bindings builder
 * @param[out] context Optional, stores error information
 * @param[out] value Bsd value to modify
 * @param[in] b bindings builder to use. Make sure to unref this afterwards.
 * @return error code, NIX_OK on success.
 */
bsd_err bsd_make_attrs(bsd_c_context * context, bsd_value * value, BindingsBuilder * b);

/** @brief Set primop
 * @param[out] context Optional, stores error information
 * @param[out] value Bsd value to modify
 * @param[in] op primop, will be gc-referenced by the value
 * @see bsd_alloc_primop
 * @return error code, NIX_OK on success.
 */
bsd_err bsd_init_primop(bsd_c_context * context, bsd_value * value, PrimOp * op);
/** @brief Copy from another value
 * @param[out] context Optional, stores error information
 * @param[out] value Bsd value to modify
 * @param[in] source value to copy from
 * @return error code, NIX_OK on success.
 */
bsd_err bsd_copy_value(bsd_c_context * context, bsd_value * value, const bsd_value * source);
/**@}*/

/** @brief Create a bindings builder
* @param[out] context Optional, stores error information
* @param[in] state bsd evaluator state
* @param[in] capacity how many bindings you'll add. Don't exceed.
* @return owned reference to a bindings builder. Make sure to unref when you're
done.
*/
BindingsBuilder * bsd_make_bindings_builder(bsd_c_context * context, EvalState * state, size_t capacity);

/** @brief Insert bindings into a builder
 * @param[out] context Optional, stores error information
 * @param[in] builder BindingsBuilder to insert into
 * @param[in] name attribute name, only used for the duration of the call.
 * @param[in] value value to give the binding
 * @return error code, NIX_OK on success.
 */
bsd_err
bsd_bindings_builder_insert(bsd_c_context * context, BindingsBuilder * builder, const char * name, bsd_value * value);

/** @brief Free a bindings builder
 *
 * Does not fail.
 * @param[in] builder the builder to free
 */
void bsd_bindings_builder_free(BindingsBuilder * builder);
/**@}*/

/** @brief Realise a string context.
 *
 * This will
 *  - realise the store paths referenced by the string's context, and
 *  - perform the replacement of placeholders.
 *  - create temporary garbage collection roots for the store paths, for
 *    the lifetime of the current process.
 *  - log to stderr
 *
 * @param[out] context Optional, stores error information
 * @param[in] value Bsd value, which must be a string
 * @param[in] state Bsd evaluator state
 * @param[in] isIFD If true, disallow derivation outputs if setting `allow-import-from-derivation` is false.
                    You should set this to true when this call is part of a primop.
                    You should set this to false when building for your application's purpose.
 * @return NULL if failed, are a new bsd_realised_string, which must be freed with bsd_realised_string_free
 */
bsd_realised_string * bsd_string_realise(bsd_c_context * context, EvalState * state, bsd_value * value, bool isIFD);

/** @brief Start of the string
 * @param[in] realised_string
 * @return pointer to the start of the string. It may not be null-terminated.
 */
const char * bsd_realised_string_get_buffer_start(bsd_realised_string * realised_string);

/** @brief Length of the string
 * @param[in] realised_string
 * @return length of the string in bytes
 */
size_t bsd_realised_string_get_buffer_size(bsd_realised_string * realised_string);

/** @brief Number of realised store paths
 * @param[in] realised_string
 * @return number of realised store paths that were referenced by the string via its context
 */
size_t bsd_realised_string_get_store_path_count(bsd_realised_string * realised_string);

/** @brief Get a store path. The store paths are stored in an arbitrary order.
 * @param[in] realised_string
 * @param[in] index index of the store path, must be less than the count
 * @return store path
 */
const StorePath * bsd_realised_string_get_store_path(bsd_realised_string * realised_string, size_t index);

/** @brief Free a realised string
 * @param[in] realised_string
 */
void bsd_realised_string_free(bsd_realised_string * realised_string);

// cffi end
#ifdef __cplusplus
}
#endif

/** @} */
#endif // NIX_API_VALUE_H
