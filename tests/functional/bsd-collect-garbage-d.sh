#!/usr/bin/env bash

source common.sh

TODO_BasedLinux

clearStore

## Test `bsd-collect-garbage -d`

# TODO make `bsd-env` doesn't work with CA derivations, and make
# `ca/bsd-collect-garbage-d.sh` wrapper.

testCollectGarbageD () {
    clearProfiles
    # Run two `bsd-env` commands, should create two generations of
    # the profile
    bsd-env -f ./user-envs.bsd -i foo-1.0 "$@"
    bsd-env -f ./user-envs.bsd -i foo-2.0pre1 "$@"
    [[ $(bsd-env --list-generations "$@" | wc -l) -eq 2 ]]

    # Clear the profile history. There should be only one generation
    # left
    bsd-collect-garbage -d
    [[ $(bsd-env --list-generations "$@" | wc -l) -eq 1 ]]
}

testCollectGarbageD

# Run the same test, but forcing the profiles an arbitrary location.
rm ~/.bsd-profile
ln -s $TEST_ROOT/blah ~/.bsd-profile
testCollectGarbageD

# Run the same test, but forcing the profiles at their legacy location under
# /bsd/var/bsd.
#
# Note that we *don't* use the default profile; `bsd-collect-garbage` will
# need to check the legacy conditional unconditionally not just follow
# `~/.bsd-profile` to pass this test.
#
# Regression test for #8294
rm ~/.bsd-profile
testCollectGarbageD --profile "$NIX_STATE_DIR/profiles/per-user/me"
