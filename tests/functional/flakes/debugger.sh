#!/usr/bin/env bash

source ./common.sh

requireGit

flakeDir="$TEST_ROOT/flake"
createGitRepo "$flakeDir"

cat >"$flakeDir/flake.bsd" <<EOF
{
  inputs = {
  };

  outputs =
    _:
    let
    in
    {
      packages.$system.default = throw "oh no";
    };
}
EOF

git -C "$flakeDir" add flake.bsd

# regression #12527 and #11286
echo ":env" | expect 1 bsd eval "$flakeDir#packages.${system}.default" --debugger
