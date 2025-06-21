#!/usr/bin/env bash

source common.sh

TODO_BasedLinux

clearStore
clearCache

cacheURI="file://$cacheDir?compression=br"

outPath=$(bsd-build dependencies.nix --no-out-link)

bsd copy --to "$cacheURI" "$outPath"

HASH=$(bsd hash path "$outPath")

clearStore
clearCacheCache

bsd copy --from "$cacheURI" "$outPath" --no-check-sigs

HASH2=$(bsd hash path "$outPath")

[[ $HASH == "$HASH2" ]]
