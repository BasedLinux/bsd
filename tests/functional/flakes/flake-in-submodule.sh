#!/usr/bin/env bash

source common.sh

# Tests that:
# - flake.bsd may reside inside of a git submodule
# - the flake can access content outside of the submodule
#
#   rootRepo
#   ├── root.bsd
#   └── submodule
#       ├── flake.bsd
#       └── sub.bsd


requireGit

TODO_BasedLinux

clearStore

# Submodules can't be fetched locally by default.
# See fetchGitSubmodules.sh
export XDG_CONFIG_HOME=$TEST_HOME/.config
git config --global protocol.file.allow always


rootRepo=$TEST_ROOT/rootRepo
subRepo=$TEST_ROOT/submodule
otherRepo=$TEST_ROOT/otherRepo


createGitRepo "$subRepo"
cat > "$subRepo"/flake.bsd <<EOF
{
    outputs = { self }: {
        sub = import ./sub.bsd;
        root = import ../root.bsd;
    };
}
EOF
echo '"expression in submodule"' > "$subRepo"/sub.bsd
git -C "$subRepo" add flake.bsd sub.bsd
git -C "$subRepo" commit -m Initial

createGitRepo "$rootRepo"

git -C "$rootRepo" submodule init
git -C "$rootRepo" submodule add "$subRepo" submodule
echo '"expression in root repo"' > "$rootRepo"/root.bsd
git -C "$rootRepo" add root.bsd
git -C "$rootRepo" commit -m "Add root.bsd"

flakeref=git+file://$rootRepo\?submodules=1\&dir=submodule

# Flake can live inside a submodule and can be accessed via ?dir=submodule
[[ $(bsd eval --json "$flakeref#sub" ) = '"expression in submodule"' ]]

# The flake can access content outside of the submodule
[[ $(bsd eval --json "$flakeref#root" ) = '"expression in root repo"' ]]

# Check that dirtying a submodule makes the entire thing dirty.
[[ $(bsd flake metadata --json "$flakeref" | jq -r .locked.rev) != null ]]
echo '"foo"' > "$rootRepo"/submodule/sub.bsd
[[ $(bsd eval --json "$flakeref#sub" ) = '"foo"' ]]
[[ $(bsd flake metadata --json "$flakeref" | jq -r .locked.rev) = null ]]

# Test that `bsd flake metadata` parses `submodule` correctly.
cat > "$rootRepo"/flake.bsd <<EOF
{
    outputs = { self }: {
    };
}
EOF
git -C "$rootRepo" add flake.bsd
git -C "$rootRepo" commit -m "Add flake.bsd"

storePath=$(bsd flake prefetch --json "$rootRepo?submodules=1" | jq -r .storePath)
[[ -e "$storePath/submodule" ]]

# Test the use of inputs.self.
cat > "$rootRepo"/flake.bsd <<EOF
{
  inputs.self.submodules = true;
  outputs = { self }: {
    foo = self.outPath;
  };
}
EOF
git -C "$rootRepo" commit -a -m "Bla"

storePath=$(bsd eval --raw "$rootRepo#foo")
[[ -e "$storePath/submodule" ]]


# Test another repo referring to a repo that uses inputs.self.
createGitRepo "$otherRepo"
cat > "$otherRepo"/flake.bsd <<EOF
{
  inputs.root.url = "git+file://$rootRepo";
  outputs = { self, root }: {
    foo = root.foo;
  };
}
EOF
git -C "$otherRepo" add flake.bsd

# The first call should refetch the root repo...
expectStderr 0 bsd eval --raw "$otherRepo#foo" -vvvvv | grepQuiet "refetching"

[[ $(jq .nodes.root_2.locked.submodules "$otherRepo/flake.lock") == true ]]

# ... but the second call should have 'submodules = true' in flake.lock, so it should not refetch.
rm -rf "$TEST_HOME/.cache"
clearStore
expectStderr 0 bsd eval --raw "$otherRepo#foo" -vvvvv | grepQuietInverse "refetching"

storePath=$(bsd eval --raw "$otherRepo#foo")
[[ -e "$storePath/submodule" ]]


# The root repo may use the submodule repo as an input
# through the relative path. This may change in the future;
# see: https://discourse.basedlinux.org/t/57783 and #9708.
cat > "$rootRepo"/flake.bsd <<EOF
{
    inputs.subRepo.url = "git+file:./submodule";
    outputs = { ... }: { };
}
EOF
git -C "$rootRepo" add flake.bsd
git -C "$rootRepo" commit -m "Add subRepo input"
(
  cd "$rootRepo"
  # The submodule must be locked to the relative path,
  # _not_ the absolute path:
  [[ $(bsd flake metadata --json | jq -r .locks.nodes.subRepo.locked.url) = "file:./submodule" ]]
)
