#ifndef NIX_API_FETCHERS_H
#define NIX_API_FETCHERS_H
/** @defgroup libfetchers libfetchers
 * @brief Bindings to the Bsd fetchers library
 * @{
 */
/** @file
 * @brief Main entry for the libfetchers C bindings
 */

#include "bsd_api_util.h"

#ifdef __cplusplus
extern "C" {
#endif
// cffi start

// Type definitions
/**
 * @brief Shared settings object
 */
typedef struct bsd_fetchers_settings bsd_fetchers_settings;

bsd_fetchers_settings * bsd_fetchers_settings_new(bsd_c_context * context);

void bsd_fetchers_settings_free(bsd_fetchers_settings * settings);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // NIX_API_FETCHERS_H