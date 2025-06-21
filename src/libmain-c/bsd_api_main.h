#ifndef NIX_API_MAIN_H
#define NIX_API_MAIN_H
/**
 * @defgroup libmain libmain
 * @brief C bindings for bsd libmain
 *
 * libmain has misc utilities for CLI commands
 * @{
 */
/** @file
 * @brief Main entry for the libmain C bindings
 */

#include "bsd_api_util.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif
// cffi start

/**
 * @brief Loads the plugins specified in Bsd's plugin-files setting.
 *
 * Call this once, after calling your desired init functions and setting
 * relevant settings.
 *
 * @param[out] context Optional, stores error information
 * @return NIX_OK if the initialization was successful, an error code otherwise.
 */
bsd_err bsd_init_plugins(bsd_c_context * context);

// cffi end
#ifdef __cplusplus
}
#endif
/**
 * @}
 */
#endif // NIX_API_MAIN_H
