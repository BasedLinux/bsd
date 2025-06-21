#!/usr/bin/env bash

source common.sh

drv="$(bsd-instantiate simple.bsd)"
cat "$drv"
out="$("${_NIX_TEST_BUILD_DIR}/test-libstoreconsumer/test-libstoreconsumer" "$drv")"
grep -F "Hello World!" < "$out/hello"
