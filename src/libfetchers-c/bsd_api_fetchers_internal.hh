#pragma once
#include "bsd/fetchers/fetch-settings.hh"
#include "bsd/util/ref.hh"

/**
 * A shared reference to `bsd::fetchers::Settings`
 * @see bsd::fetchers::Settings
 */
struct bsd_fetchers_settings
{
    bsd::ref<bsd::fetchers::Settings> settings;
};
