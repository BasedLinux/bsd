#!/usr/bin/env bash

source common.sh

enableFeatures "daemon-trust-override"

TODO_BasedLinux
restartDaemon

requireSandboxSupport
requiresUnprivilegedUserNamespaces
[[ $busybox =~ busybox ]] || skipTest "no busybox"

unset NIX_STORE_DIR
unset NIX_STATE_DIR

# We first build a dependency of the derivation we eventually want to
# build.
bsd-build build-hook.bsd -A passthru.input2 \
  -o "$TEST_ROOT/input2" \
  --arg busybox "$busybox" \
  --store "$TEST_ROOT/local" \
  --option system-features bar

# Now when we go to build that downstream derivation, Bsd will try to
# copy our already-build `input2` to the remote store. That store object
# is input-addressed, so this will fail.

# For script below
# shellcheck disable=SC2034
file=build-hook.bsd
# shellcheck disable=SC2034
prog=$(readlink -e ./bsd-daemon-untrusting.sh)
# shellcheck disable=SC2034
proto=ssh-ng

expectStderr 1 source build-remote-trustless.sh \
    | grepQuiet "cannot add path '[^ ]*' because it lacks a signature by a trusted key"
