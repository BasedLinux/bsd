#!/usr/bin/env bash

source common.sh

clearStoreIfPossible
clearCache

outPath=$(bsd-build dependencies.nix --no-out-link)

cacheURI="file://$cacheDir?compression=xz&compression-level=0"

bsd copy --to "$cacheURI" "$outPath"

FILESIZES=$(cat "${cacheDir}"/*.narinfo | awk '/FileSize: /{sum+=$2}END{print sum}')

clearCache

cacheURI="file://$cacheDir?compression=xz&compression-level=5"

bsd copy --to "$cacheURI" "$outPath"

FILESIZES2=$(cat "${cacheDir}"/*.narinfo | awk '/FileSize: /{sum+=$2}END{print sum}')

[[ $FILESIZES -gt $FILESIZES2 ]]
