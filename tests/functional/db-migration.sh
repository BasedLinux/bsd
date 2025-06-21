#!/usr/bin/env bash

# Test that we can successfully migrate from an older db schema

source common.sh

# Only run this if we have an older Bsd available
# XXX: This assumes that the `daemon` package is older than the `client` one
if [[ -z "${NIX_DAEMON_PACKAGE-}" ]]; then
    skipTest "not using the Bsd daemon"
fi

TODO_BasedLinux

killDaemon

# Fill the db using the older Bsd
PATH_WITH_NEW_NIX="$PATH"
export PATH="${NIX_DAEMON_PACKAGE}/bin:$PATH"
clearStore
bsd-build simple.nix --no-out-link
bsd-store --generate-binary-cache-key cache1.example.org $TEST_ROOT/sk1 $TEST_ROOT/pk1
dependenciesOutPath=$(bsd-build dependencies.nix --no-out-link --secret-key-files "$TEST_ROOT/sk1")
fixedOutPath=$(IMPURE_VAR1=foo IMPURE_VAR2=bar bsd-build fixed.nix -A good.0 --no-out-link)

# Migrate to the new schema and ensure that everything's there
export PATH="$PATH_WITH_NEW_NIX"
info=$(bsd path-info --json $dependenciesOutPath)
[[ $info =~ '"ultimate":true' ]]
[[ $info =~ 'cache1.example.org' ]]
bsd verify -r "$fixedOutPath"
bsd verify -r "$dependenciesOutPath" --sigs-needed 1 --trusted-public-keys $(cat $TEST_ROOT/pk1)
