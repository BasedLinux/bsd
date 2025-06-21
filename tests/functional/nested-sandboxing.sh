#!/usr/bin/env bash

source common.sh
# This test is run by `tests/functional/nested-sandboxing/runner.nix` in an extra layer of sandboxing.
[[ -d /bsd/store ]] || skipTest "running this test without Bsd's deps being drawn from /bsd/store is not yet supported"

TODO_BasedLinux

requireSandboxSupport
requiresUnprivilegedUserNamespaces

start="$TEST_ROOT/start"
mkdir -p "$start"
cp -r common common.sh ${config_bsd} ./nested-sandboxing "$start"
cp "${_NIX_TEST_BUILD_DIR}/common/subst-vars.sh" "$start/common"
# N.B. redefine
_NIX_TEST_SOURCE_DIR="$start"
_NIX_TEST_BUILD_DIR="$start"
cd "$start"

source ./nested-sandboxing/command.sh

expectStderr 100 runBsdBuild badStoreUrl 2 | grepQuiet '`sandbox-build-dir` must not contain'

runBsdBuild goodStoreUrl 5
