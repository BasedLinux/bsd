#!/usr/bin/env bash

source common.sh

source bsd-copy-ssh-common.sh "ssh-ng"

TODO_BasedLinux

clearStore
clearRemoteStore

outPath=$(bsd-build --no-out-link dependencies.nix)

bsd store info --store "$remoteStore"

# Regression test for https://github.com/BasedLinux/bsd/issues/6253
bsd copy --to "$remoteStore" $outPath --no-check-sigs &
bsd copy --to "$remoteStore" $outPath --no-check-sigs
