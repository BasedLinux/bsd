#include "bsd_api_store.h"
#include "bsd_api_store_internal.h"
#include "bsd_api_util.h"
#include "bsd_api_util_internal.h"

#include "bsd/main/plugin.hh"

bsd_err bsd_init_plugins(bsd_c_context * context)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        bsd::initPlugins();
    }
    NIXC_CATCH_ERRS
}
