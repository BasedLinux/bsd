#ifndef NIX_API_EXTERNAL_H
#define NIX_API_EXTERNAL_H
/** @ingroup libexpr
 * @addtogroup Externals
 * @brief Deal with external values
 * @{
 */
/** @file
 * @brief libexpr C bindings dealing with external values
 */

#include "bsd_api_expr.h"
#include "bsd_api_util.h"
#include "bsd_api_value.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
// cffi start

/**
 * @brief Represents a string owned by the Bsd language evaluator.
 * @see bsd_set_owned_string
 */
typedef struct bsd_string_return bsd_string_return;
/**
 * @brief Wraps a stream that can output multiple string pieces.
 */
typedef struct bsd_printer bsd_printer;
/**
 * @brief A list of string context items
 */
typedef struct bsd_string_context bsd_string_context;

/**
 * @brief Sets the contents of a bsd_string_return
 *
 * Copies the passed string.
 * @param[out] str the bsd_string_return to write to
 * @param[in]  c   The string to copy
 */
void bsd_set_string_return(bsd_string_return * str, const char * c);

/**
 * Print to the bsd_printer
 *
 * @param[out] context Optional, stores error information
 * @param[out] printer The bsd_printer to print to
 * @param[in] str The string to print
 * @returns NIX_OK if everything worked
 */
bsd_err bsd_external_print(bsd_c_context * context, bsd_printer * printer, const char * str);

/**
 * Add string context to the bsd_string_context object
 * @param[out] context Optional, stores error information
 * @param[out] string_context The bsd_string_context to add to
 * @param[in] c The context string to add
 * @returns NIX_OK if everything worked
 */
bsd_err bsd_external_add_string_context(bsd_c_context * context, bsd_string_context * string_context, const char * c);

/**
 * @brief Definition for a class of external values
 *
 * Create and implement one of these, then pass it to bsd_create_external_value
 * Make sure to keep it alive while the external value lives.
 *
 * Optional functions can be set to NULL
 *
 * @see bsd_create_external_value
 */
typedef struct BsdCExternalValueDesc
{
    /**
     * @brief Called when printing the external value
     *
     * @param[in] self the void* passed to bsd_create_external_value
     * @param[out] printer The printer to print to, pass to bsd_external_print
     */
    void (*print)(void * self, bsd_printer * printer);
    /**
     * @brief Called on :t
     * @param[in] self the void* passed to bsd_create_external_value
     * @param[out] res the return value
     */
    void (*showType)(void * self, bsd_string_return * res);
    /**
     * @brief Called on `builtins.typeOf`
     * @param self the void* passed to bsd_create_external_value
     * @param[out] res the return value
     */
    void (*typeOf)(void * self, bsd_string_return * res);
    /**
     * @brief Called on "${str}" and builtins.toString.
     *
     * The latter with coerceMore=true
     * Optional, the default is to throw an error.
     * @param[in] self the void* passed to bsd_create_external_value
     * @param[out] c writable string context for the resulting string
     * @param[in] coerceMore boolean, try to coerce to strings in more cases
     * instead of throwing an error
     * @param[in] copyToStore boolean, whether to copy referenced paths to store
     * or keep them as-is
     * @param[out] res the return value. Not touching this, or setting it to the
     * empty string, will make the conversion throw an error.
     */
    void (*coerceToString)(
        void * self, bsd_string_context * c, int coerceMore, int copyToStore, bsd_string_return * res);
    /**
     * @brief Try to compare two external values
     *
     * Optional, the default is always false.
     * If the other object was not a Bsd C external value, this comparison will
     * also return false
     * @param[in] self the void* passed to bsd_create_external_value
     * @param[in] other the void* passed to the other object's
     * bsd_create_external_value
     * @returns true if the objects are deemed to be equal
     */
    int (*equal)(void * self, void * other);
    /**
     * @brief Convert the external value to json
     *
     * Optional, the default is to throw an error
     * @param[in] self the void* passed to bsd_create_external_value
     * @param[in] state The evaluator state
     * @param[in] strict boolean Whether to force the value before printing
     * @param[out] c writable string context for the resulting string
     * @param[in] copyToStore whether to copy referenced paths to store or keep
     * them as-is
     * @param[out] res the return value. Gets parsed as JSON. Not touching this,
     * or setting it to the empty string, will make the conversion throw an error.
     */
    void (*printValueAsJSON)(
        void * self, EvalState * state, bool strict, bsd_string_context * c, bool copyToStore, bsd_string_return * res);
    /**
     * @brief Convert the external value to XML
     *
     * Optional, the default is to throw an error
     * @todo The mechanisms for this call are incomplete. There are no C
     *       bindings to work with XML, pathsets and positions.
     * @param[in] self the void* passed to bsd_create_external_value
     * @param[in] state The evaluator state
     * @param[in] strict boolean Whether to force the value before printing
     * @param[in] location boolean Whether to include position information in the
     * xml
     * @param[out] doc XML document to output to
     * @param[out] c writable string context for the resulting string
     * @param[in,out] drvsSeen a path set to avoid duplicating derivations
     * @param[in] pos The position of the call.
     */
    void (*printValueAsXML)(
        void * self,
        EvalState * state,
        int strict,
        int location,
        void * doc,
        bsd_string_context * c,
        void * drvsSeen,
        int pos);
} BsdCExternalValueDesc;

/**
 * @brief Create an external value, that can be given to bsd_init_external
 *
 * Owned by the GC. Use bsd_gc_decref when you're done with the pointer.
 *
 * @param[out] context Optional, stores error information
 * @param[in] desc a BsdCExternalValueDesc, you should keep this alive as long
 * as the ExternalValue lives
 * @param[in] v the value to store
 * @returns external value, owned by the garbage collector
 * @see bsd_init_external
 */
ExternalValue * bsd_create_external_value(bsd_c_context * context, BsdCExternalValueDesc * desc, void * v);

/**
 * @brief Extract the pointer from a bsd c external value.
 * @param[out] context Optional, stores error information
 * @param[in] b The external value
 * @returns The pointer, or null if the external value was not from bsd c.
 * @see bsd_get_external
 */
void * bsd_get_external_value_content(bsd_c_context * context, ExternalValue * b);

// cffi end
#ifdef __cplusplus
}
#endif
/** @} */

#endif // NIX_API_EXTERNAL_H
