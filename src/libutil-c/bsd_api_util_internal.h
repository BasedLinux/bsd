#ifndef NIX_API_UTIL_INTERNAL_H
#define NIX_API_UTIL_INTERNAL_H

#include <string>
#include <optional>

#include "bsd/util/error.hh"
#include "bsd_api_util.h"

struct bsd_c_context
{
    bsd_err last_err_code = NIX_OK;
    /** The last error message. Always check last_err_code. This may not have been cleared, so that clearing is fast. */
    std::optional<std::string> last_err = {};
    std::optional<bsd::ErrorInfo> info = {};
    std::string name = "";
};

bsd_err bsd_context_error(bsd_c_context * context);

/**
 * Internal use only.
 *
 * Helper to invoke bsd_get_string_callback
 * @param context optional, the context to store errors in if this function
 * fails
 * @param str The string to observe
 * @param callback Called with the observed string.
 * @param user_data optional, arbitrary data, passed to the callback when it's called.
 * @return NIX_OK if there were no errors.
 * @see bsd_get_string_callback
 */
bsd_err call_bsd_get_string_callback(const std::string str, bsd_get_string_callback callback, void * user_data);

#define NIXC_CATCH_ERRS \
    catch (...) \
    { \
        return bsd_context_error(context); \
    } \
    return NIX_OK;

#define NIXC_CATCH_ERRS_RES(def) \
    catch (...) \
    { \
        bsd_context_error(context); \
        return def; \
    }
#define NIXC_CATCH_ERRS_NULL NIXC_CATCH_ERRS_RES(nullptr)

#endif // NIX_API_UTIL_INTERNAL_H
