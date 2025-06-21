#!/usr/bin/env bash

source ./common.sh

requireGit

flake1Dir=$TEST_ROOT/flake1
flake2Dir=$TEST_ROOT/flake2

createGitRepo "$flake1Dir"
cat > "$flake1Dir"/flake.bsd <<EOF
{
    outputs = { self }: { x = import ./x.bsd; };
}
EOF
echo 123 > "$flake1Dir"/x.bsd
git -C "$flake1Dir" add flake.bsd x.bsd
git -C "$flake1Dir" commit -m Initial

createGitRepo "$flake2Dir"
cat > "$flake2Dir"/flake.bsd <<EOF
{
    outputs = { self, flake1 }: { x = flake1.x; };
}
EOF
git -C "$flake2Dir" add flake.bsd

[[ $(bsd eval --json "$flake2Dir#x" --override-input flake1 "$TEST_ROOT/flake1") = 123 ]]

echo 456 > "$flake1Dir"/x.bsd

[[ $(bsd eval --json "$flake2Dir#x" --override-input flake1 "$TEST_ROOT/flake1") = 456 ]]

# Dirty overrides require --allow-dirty-locks.
expectStderr 1 bsd flake lock "$flake2Dir" --override-input flake1 "$TEST_ROOT/flake1" |
  grepQuiet "Not writing lock file.*because it has an unlocked input"

bsd flake lock "$flake2Dir" --override-input flake1 "$TEST_ROOT/flake1" --allow-dirty-locks

# Using a lock file with a dirty lock does not require --allow-dirty-locks, but should print a warning.
expectStderr 0 bsd eval "$flake2Dir#x" |
  grepQuiet "warning: Lock file entry .* is unlocked"

[[ $(bsd eval "$flake2Dir#x") = 456 ]]
