#!/usr/bin/env bash

source common.sh

TODO_BasedLinux

clearStoreIfPossible
clearCacheCache

# Fails without remote builders
(! bsd-build --store "file://$cacheDir" dependencies.nix)

# Succeeds with default store as build remote.
outPath=$(bsd-build --store "file://$cacheDir" --builders 'auto - - 1 1' -j0 dependencies.nix)

# Test that the path exactly exists in the destination store.
bsd path-info --store "file://$cacheDir" "$outPath"

# Succeeds without any build capability because no-op
bsd-build --store "file://$cacheDir" -j0 dependencies.nix
