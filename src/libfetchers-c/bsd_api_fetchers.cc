#include "bsd_api_fetchers.h"
#include "bsd_api_fetchers_internal.hh"
#include "bsd_api_util_internal.h"

bsd_fetchers_settings * bsd_fetchers_settings_new(bsd_c_context * context)
{
    try {
        auto fetchersSettings = bsd::make_ref<bsd::fetchers::Settings>(bsd::fetchers::Settings{});
        return new bsd_fetchers_settings{
            .settings = fetchersSettings,
        };
    }
    NIXC_CATCH_ERRS_NULL
}

void bsd_fetchers_settings_free(bsd_fetchers_settings * settings)
{
    delete settings;
}
