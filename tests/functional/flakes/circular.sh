#!/usr/bin/env bash

# Test circular flake dependencies.
source ./common.sh

requireGit

flakeA=$TEST_ROOT/flakeA
flakeB=$TEST_ROOT/flakeB

createGitRepo "$flakeA"
createGitRepo "$flakeB"

cat > "$flakeA"/flake.bsd <<EOF
{
  inputs.b.url = git+file://$flakeB;
  inputs.b.inputs.a.follows = "/";

  outputs = { self, b }: {
    foo = 123 + b.bar;
    xyzzy = 1000;
  };
}
EOF

git -C "$flakeA" add flake.bsd

cat > "$flakeB"/flake.bsd <<EOF
{
  inputs.a.url = git+file://$flakeA;

  outputs = { self, a }: {
    bar = 456 + a.xyzzy;
  };
}
EOF

git -C "$flakeB" add flake.bsd
git -C "$flakeB" commit -a -m 'Foo'

[[ $(bsd eval "$flakeA#foo") = 1579 ]]
[[ $(bsd eval "$flakeA#foo") = 1579 ]]

sed -i "$flakeB"/flake.bsd -e 's/456/789/'
git -C "$flakeB" commit -a -m 'Foo'

bsd flake update b --flake "$flakeA"
[[ $(bsd eval "$flakeA#foo") = 1912 ]]

# Test list-inputs with circular dependencies
bsd flake metadata "$flakeA"

