#!/usr/bin/env bash

set -eu -o pipefail

source common.sh

# Avoid store dir being inside sandbox build-dir
unset NIX_STORE_DIR
unset NIX_STATE_DIR

setupStoreDirs

initLowerStore

mountOverlayfs

export NIX_REMOTE="$storeB"
stateB="$storeBRoot/bsd/var/bsd"
outPath=$(bsd-build ../hermetic.nix --no-out-link --arg busybox "$busybox" --arg seed 2)

# Set a GC root.
mkdir -p "$stateB"
rm -f "$stateB/gcroots/foo"
ln -sf $outPath "$stateB/gcroots/foo"

[ "$(bsd-store -q --roots $outPath)" = "$stateB/gcroots/foo -> $outPath" ]

bsd-store --gc --print-roots | grep $outPath
bsd-store --gc --print-live | grep $outPath
if bsd-store --gc --print-dead | grep -E $outPath$; then false; fi

bsd-store --gc --print-dead

expect 1 bsd-store --delete $outPath
test -e "$storeBRoot/$outPath"

shopt -s nullglob
for i in $storeBRoot/*; do
    if [[ $i =~ /trash ]]; then continue; fi # compat with old daemon
    touch $i.lock
    touch $i.chroot
done

bsd-collect-garbage

# Check that the root and its dependencies haven't been deleted.
cat "$storeBRoot/$outPath"

rm "$stateB/gcroots/foo"

bsd-collect-garbage

# Check that the output has been GC'd.
test ! -e $outPath

# Check that the store is empty.
[ "$(ls -1 "$storeBTop" | wc -l)" = "0" ]
