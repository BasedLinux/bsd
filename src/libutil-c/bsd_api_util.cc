#include "bsd_api_util.h"
#include "bsd/util/config-global.hh"
#include "bsd/util/error.hh"
#include "bsd_api_util_internal.h"
#include "bsd/util/util.hh"

#include <cxxabi.h>
#include <typeinfo>

#include "bsd_api_util_config.h"

bsd_c_context * bsd_c_context_create()
{
    return new bsd_c_context();
}

void bsd_c_context_free(bsd_c_context * context)
{
    delete context;
}

bsd_err bsd_context_error(bsd_c_context * context)
{
    if (context == nullptr) {
        throw;
    }
    try {
        throw;
    } catch (bsd::Error & e) {
        /* Storing this exception is annoying, take what we need here */
        context->last_err = e.what();
        context->info = e.info();
        int status;
        const char * demangled = abi::__cxa_demangle(typeid(e).name(), 0, 0, &status);
        if (demangled) {
            context->name = demangled;
            // todo: free(demangled);
        } else {
            context->name = typeid(e).name();
        }
        context->last_err_code = NIX_ERR_NIX_ERROR;
        return context->last_err_code;
    } catch (const std::exception & e) {
        context->last_err = e.what();
        context->last_err_code = NIX_ERR_UNKNOWN;
        return context->last_err_code;
    }
    // unreachable
}

bsd_err bsd_set_err_msg(bsd_c_context * context, bsd_err err, const char * msg)
{
    if (context == nullptr) {
        // todo last_err_code
        throw bsd::Error("Bsd C api error: %s", msg);
    }
    context->last_err_code = err;
    context->last_err = msg;
    return err;
}

void bsd_clear_err(bsd_c_context * context)
{
    if (context)
        context->last_err_code = NIX_OK;
}

const char * bsd_version_get()
{
    return PACKAGE_VERSION;
}

// Implementations

bsd_err bsd_setting_get(bsd_c_context * context, const char * key, bsd_get_string_callback callback, void * user_data)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        std::map<std::string, bsd::AbstractConfig::SettingInfo> settings;
        bsd::globalConfig.getSettings(settings);
        if (settings.contains(key)) {
            return call_bsd_get_string_callback(settings[key].value, callback, user_data);
        } else {
            return bsd_set_err_msg(context, NIX_ERR_KEY, "Setting not found");
        }
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_setting_set(bsd_c_context * context, const char * key, const char * value)
{
    if (context)
        context->last_err_code = NIX_OK;
    if (bsd::globalConfig.set(key, value))
        return NIX_OK;
    else {
        return bsd_set_err_msg(context, NIX_ERR_KEY, "Setting not found");
    }
}

bsd_err bsd_libutil_init(bsd_c_context * context)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        bsd::initLibUtil();
        return NIX_OK;
    }
    NIXC_CATCH_ERRS
}

const char * bsd_err_msg(bsd_c_context * context, const bsd_c_context * read_context, unsigned int * n)
{
    if (context)
        context->last_err_code = NIX_OK;
    if (read_context->last_err && read_context->last_err_code != NIX_OK) {
        if (n)
            *n = read_context->last_err->size();
        return read_context->last_err->c_str();
    }
    bsd_set_err_msg(context, NIX_ERR_UNKNOWN, "No error message");
    return nullptr;
}

bsd_err bsd_err_name(
    bsd_c_context * context, const bsd_c_context * read_context, bsd_get_string_callback callback, void * user_data)
{
    if (context)
        context->last_err_code = NIX_OK;
    if (read_context->last_err_code != NIX_ERR_NIX_ERROR) {
        return bsd_set_err_msg(context, NIX_ERR_UNKNOWN, "Last error was not a bsd error");
    }
    return call_bsd_get_string_callback(read_context->name, callback, user_data);
}

bsd_err bsd_err_info_msg(
    bsd_c_context * context, const bsd_c_context * read_context, bsd_get_string_callback callback, void * user_data)
{
    if (context)
        context->last_err_code = NIX_OK;
    if (read_context->last_err_code != NIX_ERR_NIX_ERROR) {
        return bsd_set_err_msg(context, NIX_ERR_UNKNOWN, "Last error was not a bsd error");
    }
    return call_bsd_get_string_callback(read_context->info->msg.str(), callback, user_data);
}

bsd_err bsd_err_code(const bsd_c_context * read_context)
{
    return read_context->last_err_code;
}

// internal
bsd_err call_bsd_get_string_callback(const std::string str, bsd_get_string_callback callback, void * user_data)
{
    callback(str.c_str(), str.size(), user_data);
    return NIX_OK;
}
