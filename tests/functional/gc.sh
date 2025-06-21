#!/usr/bin/env bash

source common.sh

TODO_BasedLinux

clearStore

drvPath=$(bsd-instantiate dependencies.nix)
outPath=$(bsd-store -rvv "$drvPath")

# Set a GC root.
rm -f "$NIX_STATE_DIR/gcroots/foo"
ln -sf "$outPath" "$NIX_STATE_DIR/gcroots/foo"

[ "$(bsd-store -q --roots "$outPath")" = "$NIX_STATE_DIR/gcroots/foo -> $outPath" ]

bsd-store --gc --print-roots | grep "$outPath"
bsd-store --gc --print-live | grep "$outPath"
bsd-store --gc --print-dead | grep "$drvPath"
if bsd-store --gc --print-dead | grep -E "$outPath"$; then false; fi

bsd-store --gc --print-dead

inUse=$(readLink "$outPath/reference-to-input-2")
if bsd-store --delete "$inUse"; then false; fi
test -e "$inUse"

if bsd-store --delete "$outPath"; then false; fi
test -e "$outPath"

for i in "$NIX_STORE_DIR"/*; do
    if [[ $i =~ /trash ]]; then continue; fi # compat with old daemon
    touch "$i.lock"
    touch "$i.chroot"
done

bsd-collect-garbage

# Check that the root and its dependencies haven't been deleted.
cat "$outPath/foobar"
cat "$outPath/reference-to-input-2/bar"

# Check that the derivation has been GC'd.
if test -e "$drvPath"; then false; fi

rm "$NIX_STATE_DIR/gcroots/foo"

bsd-collect-garbage

# Check that the output has been GC'd.
if test -e "$outPath/foobar"; then false; fi

# Check that the store is empty.
rmdir "$NIX_STORE_DIR/.links"
rmdir "$NIX_STORE_DIR"
