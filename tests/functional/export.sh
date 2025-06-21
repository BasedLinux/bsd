#!/usr/bin/env bash

source common.sh

TODO_BasedLinux

clearStore

outPath=$(bsd-build dependencies.nix --no-out-link)

bsd-store --export $outPath > $TEST_ROOT/exp

bsd-store --export $(bsd-store -qR $outPath) > $TEST_ROOT/exp_all

if bsd-store --export $outPath >/dev/full ; then
    echo "exporting to a bad file descriptor should fail"
    exit 1
fi


clearStore

if bsd-store --import < $TEST_ROOT/exp; then
    echo "importing a non-closure should fail"
    exit 1
fi


clearStore

bsd-store --import < $TEST_ROOT/exp_all

bsd-store --export $(bsd-store -qR $outPath) > $TEST_ROOT/exp_all2


clearStore

# Regression test: the derivers in exp_all2 are empty, which shouldn't
# cause a failure.
bsd-store --import < $TEST_ROOT/exp_all2
