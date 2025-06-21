#!/usr/bin/env bash

source common.sh

clearStoreIfPossible

# https://github.com/BasedLinux/bsd/issues/6572
issue_6572_independent_outputs() {
    bsd build -f multiple-outputs.nix --json independent --no-link > "$TEST_ROOT"/independent.json

    # Make sure that 'bsd build' can build a derivation that depends on both outputs of another derivation.
    p=$(bsd build -f multiple-outputs.nix use-independent --no-link --print-out-paths)
    bsd-store --delete "$p" # Clean up for next test

    # Make sure that 'bsd build' tracks input-outputs correctly when a single output is already present.
    bsd-store --delete "$(jq -r <"$TEST_ROOT"/independent.json .[0].outputs.first)"
    p=$(bsd build -f multiple-outputs.nix use-independent --no-link --print-out-paths)
    cmp "$p" <<EOF
first
second
EOF
    bsd-store --delete "$p" # Clean up for next test

    # Make sure that 'bsd build' tracks input-outputs correctly when a single output is already present.
    bsd-store --delete "$(jq -r <"$TEST_ROOT"/independent.json .[0].outputs.second)"
    p=$(bsd build -f multiple-outputs.nix use-independent --no-link --print-out-paths)
    cmp "$p" <<EOF
first
second
EOF
    bsd-store --delete "$p" # Clean up for next test
}
issue_6572_independent_outputs


# https://github.com/BasedLinux/bsd/issues/6572
issue_6572_dependent_outputs() {

    bsd build -f multiple-outputs.nix --json a --no-link > "$TEST_ROOT"/a.json

    # # Make sure that 'bsd build' can build a derivation that depends on both outputs of another derivation.
    p=$(bsd build -f multiple-outputs.nix use-a --no-link --print-out-paths)
    bsd-store --delete "$p" # Clean up for next test

    # Make sure that 'bsd build' tracks input-outputs correctly when a single output is already present.
    bsd-store --delete "$(jq -r <"$TEST_ROOT"/a.json .[0].outputs.second)"
    p=$(bsd build -f multiple-outputs.nix use-a --no-link --print-out-paths)
    cmp "$p" <<EOF
first
second
EOF
    bsd-store --delete "$p" # Clean up for next test
}
if isDaemonNewer "2.12pre0"; then
    issue_6572_dependent_outputs
fi
