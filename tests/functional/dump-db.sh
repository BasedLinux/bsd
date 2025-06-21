#!/usr/bin/env bash

source common.sh

TODO_BasedLinux

needLocalStore "--dump-db requires a local store"

clearStore

path=$(bsd-build dependencies.nix -o $TEST_ROOT/result)

deps="$(bsd-store -qR $TEST_ROOT/result)"

bsd-store --dump-db > $TEST_ROOT/dump

rm -rf $NIX_STATE_DIR/db

bsd-store --load-db < $TEST_ROOT/dump

deps2="$(bsd-store -qR $TEST_ROOT/result)"

[ "$deps" = "$deps2" ];

bsd-store --dump-db > $TEST_ROOT/dump2
cmp $TEST_ROOT/dump $TEST_ROOT/dump2
