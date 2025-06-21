#include <string>

#include "bsd_api_flake.h"
#include "bsd_api_flake_internal.hh"
#include "bsd_api_util.h"
#include "bsd_api_util_internal.h"
#include "bsd_api_expr_internal.h"
#include "bsd_api_fetchers_internal.hh"
#include "bsd_api_fetchers.h"

#include "bsd/flake/flake.hh"

bsd_flake_settings * bsd_flake_settings_new(bsd_c_context * context)
{
    bsd_clear_err(context);
    try {
        auto settings = bsd::make_ref<bsd::flake::Settings>();
        return new bsd_flake_settings{settings};
    }
    NIXC_CATCH_ERRS_NULL
}

void bsd_flake_settings_free(bsd_flake_settings * settings)
{
    delete settings;
}

bsd_err bsd_flake_settings_add_to_eval_state_builder(
    bsd_c_context * context, bsd_flake_settings * settings, bsd_eval_state_builder * builder)
{
    bsd_clear_err(context);
    try {
        settings->settings->configureEvalSettings(builder->settings);
    }
    NIXC_CATCH_ERRS
}

bsd_flake_reference_parse_flags *
bsd_flake_reference_parse_flags_new(bsd_c_context * context, bsd_flake_settings * settings)
{
    bsd_clear_err(context);
    try {
        return new bsd_flake_reference_parse_flags{
            .baseDirectory = std::nullopt,
        };
    }
    NIXC_CATCH_ERRS_NULL
}

void bsd_flake_reference_parse_flags_free(bsd_flake_reference_parse_flags * flags)
{
    delete flags;
}

bsd_err bsd_flake_reference_parse_flags_set_base_directory(
    bsd_c_context * context,
    bsd_flake_reference_parse_flags * flags,
    const char * baseDirectory,
    size_t baseDirectoryLen)
{
    bsd_clear_err(context);
    try {
        flags->baseDirectory.emplace(bsd::Path{std::string(baseDirectory, baseDirectoryLen)});
        return NIX_OK;
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_flake_reference_and_fragment_from_string(
    bsd_c_context * context,
    bsd_fetchers_settings * fetchSettings,
    bsd_flake_settings * flakeSettings,
    bsd_flake_reference_parse_flags * parseFlags,
    const char * strData,
    size_t strSize,
    bsd_flake_reference ** flakeReferenceOut,
    bsd_get_string_callback fragmentCallback,
    void * fragmentCallbackUserData)
{
    bsd_clear_err(context);
    *flakeReferenceOut = nullptr;
    try {
        std::string str(strData, strSize);

        auto [flakeRef, fragment] =
            bsd::parseFlakeRefWithFragment(*fetchSettings->settings, str, parseFlags->baseDirectory, true);
        *flakeReferenceOut = new bsd_flake_reference{bsd::make_ref<bsd::FlakeRef>(flakeRef)};
        return call_bsd_get_string_callback(fragment, fragmentCallback, fragmentCallbackUserData);
    }
    NIXC_CATCH_ERRS
}

void bsd_flake_reference_free(bsd_flake_reference * flakeReference)
{
    delete flakeReference;
}

bsd_flake_lock_flags * bsd_flake_lock_flags_new(bsd_c_context * context, bsd_flake_settings * settings)
{
    bsd_clear_err(context);
    try {
        auto lockSettings = bsd::make_ref<bsd::flake::LockFlags>(bsd::flake::LockFlags{
            .recreateLockFile = false,
            .updateLockFile = true,  // == `bsd_flake_lock_flags_set_mode_write_as_needed`
            .writeLockFile = true,   // == `bsd_flake_lock_flags_set_mode_write_as_needed`
            .failOnUnlocked = false, // == `bsd_flake_lock_flags_set_mode_write_as_needed`
            .useRegistries = false,
            .allowUnlocked = false, // == `bsd_flake_lock_flags_set_mode_write_as_needed`
            .commitLockFile = false,

        });
        return new bsd_flake_lock_flags{lockSettings};
    }
    NIXC_CATCH_ERRS_NULL
}

void bsd_flake_lock_flags_free(bsd_flake_lock_flags * flags)
{
    delete flags;
}

bsd_err bsd_flake_lock_flags_set_mode_virtual(bsd_c_context * context, bsd_flake_lock_flags * flags)
{
    bsd_clear_err(context);
    try {
        flags->lockFlags->updateLockFile = true;
        flags->lockFlags->writeLockFile = false;
        flags->lockFlags->failOnUnlocked = false;
        flags->lockFlags->allowUnlocked = true;
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_flake_lock_flags_set_mode_write_as_needed(bsd_c_context * context, bsd_flake_lock_flags * flags)
{
    bsd_clear_err(context);
    try {
        flags->lockFlags->updateLockFile = true;
        flags->lockFlags->writeLockFile = true;
        flags->lockFlags->failOnUnlocked = false;
        flags->lockFlags->allowUnlocked = true;
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_flake_lock_flags_set_mode_check(bsd_c_context * context, bsd_flake_lock_flags * flags)
{
    bsd_clear_err(context);
    try {
        flags->lockFlags->updateLockFile = false;
        flags->lockFlags->writeLockFile = false;
        flags->lockFlags->failOnUnlocked = true;
        flags->lockFlags->allowUnlocked = false;
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_flake_lock_flags_add_input_override(
    bsd_c_context * context, bsd_flake_lock_flags * flags, const char * inputPath, bsd_flake_reference * flakeRef)
{
    bsd_clear_err(context);
    try {
        auto path = bsd::flake::parseInputAttrPath(inputPath);
        flags->lockFlags->inputOverrides.emplace(path, *flakeRef->flakeRef);
        if (flags->lockFlags->writeLockFile) {
            return bsd_flake_lock_flags_set_mode_virtual(context, flags);
        }
    }
    NIXC_CATCH_ERRS
}

bsd_locked_flake * bsd_flake_lock(
    bsd_c_context * context,
    bsd_fetchers_settings * fetchSettings,
    bsd_flake_settings * flakeSettings,
    EvalState * eval_state,
    bsd_flake_lock_flags * flags,
    bsd_flake_reference * flakeReference)
{
    bsd_clear_err(context);
    try {
        eval_state->state.resetFileCache();
        auto lockedFlake = bsd::make_ref<bsd::flake::LockedFlake>(bsd::flake::lockFlake(
            *flakeSettings->settings, eval_state->state, *flakeReference->flakeRef, *flags->lockFlags));
        return new bsd_locked_flake{lockedFlake};
    }
    NIXC_CATCH_ERRS_NULL
}

void bsd_locked_flake_free(bsd_locked_flake * lockedFlake)
{
    delete lockedFlake;
}

bsd_value * bsd_locked_flake_get_output_attrs(
    bsd_c_context * context, bsd_flake_settings * settings, EvalState * evalState, bsd_locked_flake * lockedFlake)
{
    bsd_clear_err(context);
    try {
        auto v = bsd_alloc_value(context, evalState);
        bsd::flake::callFlake(evalState->state, *lockedFlake->lockedFlake, v->value);
        return v;
    }
    NIXC_CATCH_ERRS_NULL
}
