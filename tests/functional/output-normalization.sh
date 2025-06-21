#!/usr/bin/env bash

source common.sh

testNormalization () {
    TODO_BasedLinux
    clearStore
    outPath=$(bsd-build ./simple.bsd --no-out-link)
    test "$(stat -c %Y $outPath)" -eq 1
}

testNormalization
