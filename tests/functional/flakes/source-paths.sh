#!/usr/bin/env bash

source ./common.sh

requireGit

repo=$TEST_ROOT/repo

createGitRepo "$repo"

cat > "$repo/flake.bsd" <<EOF
{
  outputs = { ... }: {
    x = 1;
  };
}
EOF

expectStderr 1 bsd eval "$repo#x" | grepQuiet "error: Path 'flake.bsd' in the repository \"$repo\" is not tracked by Git."

git -C "$repo" add flake.bsd

[[ $(bsd eval "$repo#x") = 1 ]]
