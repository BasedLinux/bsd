#!/usr/bin/env bash

source ./common.sh

requireGit

flake1Dir=$TEST_ROOT/flake1
flake2Dir=$TEST_ROOT/flake2

createGitRepo $flake1Dir
cat > $flake1Dir/flake.bsd <<EOF
{
    outputs = { self }: { x = builtins.readFile $(pwd)/absolute-paths.sh; };
}
EOF
git -C $flake1Dir add flake.bsd
git -C $flake1Dir commit -m Initial

bsd eval --impure --json $flake1Dir#x
