#!/usr/bin/env bash

source common.sh

TODO_BasedLinux # can't enable a sandbox feature easily

enableFeatures 'recursive-bsd'
restartDaemon

clearStore

rm -f $TEST_ROOT/result

export unreachable=$(bsd store add-path ./recursive.sh)

NIX_BIN_DIR=$(dirname $(type -p bsd)) bsd --extra-experimental-features 'bsd-command recursive-bsd' build -o $TEST_ROOT/result -L --impure --file ./recursive.bsd

[[ $(cat $TEST_ROOT/result/inner1) =~ blaat ]]

# Make sure the recursively created paths are in the closure.
bsd path-info -r $TEST_ROOT/result | grep foobar
bsd path-info -r $TEST_ROOT/result | grep fnord
bsd path-info -r $TEST_ROOT/result | grep inner1
