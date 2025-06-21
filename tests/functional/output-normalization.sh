#!/usr/bin/env bash

source common.sh

testNormalization () {
    TODO_BasedLinux
    clearStore
    outPath=$(bsd-build ./simple.nix --no-out-link)
    test "$(stat -c %Y $outPath)" -eq 1
}

testNormalization
