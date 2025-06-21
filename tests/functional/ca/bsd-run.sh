#!/usr/bin/env bash

source common.sh

flakeDir="$TEST_HOME/flake"
mkdir -p "${flakeDir}"
cp flake.bsd "${_NIX_TEST_BUILD_DIR}/ca/config.bsd" content-addressed.bsd "${flakeDir}"

bsd run --no-write-lock-file "path:${flakeDir}#runnable"
