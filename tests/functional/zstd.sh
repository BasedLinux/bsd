#!/usr/bin/env bash

source common.sh

TODO_BasedLinux

clearStore
clearCache

cacheURI="file://$cacheDir?compression=zstd"

outPath=$(bsd-build dependencies.nix --no-out-link)

bsd copy --to "$cacheURI" "$outPath"

HASH=$(bsd hash path "$outPath")

clearStore
clearCacheCache

bsd copy --from "$cacheURI" "$outPath" --no-check-sigs --profile "$TEST_ROOT/profile" --out-link "$TEST_ROOT/result"

[[ -e $TEST_ROOT/profile ]]
[[ -e $TEST_ROOT/result ]]

if ls "$cacheDir/nar/"*.zst &> /dev/null; then
    echo "files do exist"
else
    echo "nars do not exist"
    exit 1
fi

HASH2=$(bsd hash path "$outPath")

[[ "$HASH" = "$HASH2" ]]
