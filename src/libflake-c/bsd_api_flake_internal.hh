#pragma once
#include <optional>

#include "bsd/util/ref.hh"
#include "bsd/flake/flake.hh"
#include "bsd/flake/flakeref.hh"
#include "bsd/flake/settings.hh"

struct bsd_flake_settings
{
    bsd::ref<bsd::flake::Settings> settings;
};

struct bsd_flake_reference_parse_flags
{
    std::optional<bsd::Path> baseDirectory;
};

struct bsd_flake_reference
{
    bsd::ref<bsd::FlakeRef> flakeRef;
};

struct bsd_flake_lock_flags
{
    bsd::ref<bsd::flake::LockFlags> lockFlags;
};

struct bsd_locked_flake
{
    bsd::ref<bsd::flake::LockedFlake> lockedFlake;
};
