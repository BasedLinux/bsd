#!/usr/bin/env bash

source ./common.sh

TODO_BasedLinux

requireGit

clearStore
rm -rf $TEST_HOME/.cache $TEST_HOME/.config

createFlake1
createFlake2

flake3Dir=$TEST_ROOT/flake%20
percentEncodedFlake3Dir=$TEST_ROOT/flake%2520
flake5Dir=$TEST_ROOT/flake5
flake7Dir=$TEST_ROOT/flake7
badFlakeDir=$TEST_ROOT/badFlake
flakeGitBare=$TEST_ROOT/flakeGitBare

for repo in "$flake3Dir" "$flake7Dir"; do
    createGitRepo "$repo" ""
done

cat > "$flake3Dir/flake.nix" <<EOF
{
  description = "Fnord";

  outputs = { self, flake2 }: rec {
    packages.$system.xyzzy = flake2.packages.$system.bar;

    checks = {
      xyzzy = packages.$system.xyzzy;
    };
  };
}
EOF

cat > "$flake3Dir/default.nix" <<EOF
{ x = 123; }
EOF

git -C "$flake3Dir" add flake.nix default.nix
git -C "$flake3Dir" commit -m 'Initial'

# Construct a custom registry, additionally test the --registry flag
bsd registry add --registry "$registry" flake1 "git+file://$flake1Dir"
bsd registry add --registry "$registry" flake3 "git+file://$percentEncodedFlake3Dir"
bsd registry add --registry "$registry" bsdpkgs flake1

# Test 'bsd registry list'.
[[ $(bsd registry list | wc -l) == 4 ]]
bsd registry list | grep        '^global'
bsd registry list | grepInverse '^user' # nothing in user registry

# Test 'bsd flake metadata'.
bsd flake metadata flake1
bsd flake metadata flake1 | grepQuiet 'Locked URL:.*flake1.*'

# Test 'bsd flake metadata' on a chroot store.
bsd flake metadata --store $TEST_ROOT/chroot-store flake1

# Test 'bsd flake metadata' on a local flake.
(cd "$flake1Dir" && bsd flake metadata) | grepQuiet 'URL:.*flake1.*'
(cd "$flake1Dir" && bsd flake metadata .) | grepQuiet 'URL:.*flake1.*'
bsd flake metadata "$flake1Dir" | grepQuiet 'URL:.*flake1.*'

# Test 'bsd flake metadata --json'.
json=$(bsd flake metadata flake1 --json | jq .)
[[ $(echo "$json" | jq -r .description) = 'Bla bla' ]]
[[ -d $(echo "$json" | jq -r .path) ]]
[[ $(echo "$json" | jq -r .lastModified) = $(git -C "$flake1Dir" log -n1 --format=%ct) ]]
hash1=$(echo "$json" | jq -r .revision)
[[ -n $(echo "$json" | jq -r .fingerprint) ]]

echo foo > "$flake1Dir/foo"
git -C "$flake1Dir" add $flake1Dir/foo
[[ $(bsd flake metadata flake1 --json --refresh | jq -r .dirtyRevision) == "$hash1-dirty" ]]
[[ "$(bsd flake metadata flake1 --json | jq -r .fingerprint)" != null ]]

echo -n '# foo' >> "$flake1Dir/flake.nix"
flake1OriginalCommit=$(git -C "$flake1Dir" rev-parse HEAD)
git -C "$flake1Dir" commit -a -m 'Foo'
flake1NewCommit=$(git -C "$flake1Dir" rev-parse HEAD)
hash2=$(bsd flake metadata flake1 --json --refresh | jq -r .revision)
[[ $(bsd flake metadata flake1 --json --refresh | jq -r .dirtyRevision) == "null" ]]
[[ $hash1 != $hash2 ]]

# Test 'bsd build' on a flake.
bsd build -o "$TEST_ROOT/result" flake1#foo
[[ -e "$TEST_ROOT/result/hello" ]]

# Test packages.default.
bsd build -o "$TEST_ROOT/result" flake1
[[ -e "$TEST_ROOT/result/hello" ]]

bsd build -o "$TEST_ROOT/result" "$flake1Dir"
bsd build -o "$TEST_ROOT/result" "git+file://$flake1Dir"
(cd "$flake1Dir" && bsd build -o "$TEST_ROOT/result" ".")
(cd "$flake1Dir" && bsd build -o "$TEST_ROOT/result" "path:.")
(cd "$flake1Dir" && bsd build -o "$TEST_ROOT/result" "git+file:.")

# Test explicit packages.default.
bsd build -o "$TEST_ROOT/result" "$flake1Dir#default"
bsd build -o "$TEST_ROOT/result" "git+file://$flake1Dir#default"

# Test explicit packages.default with query.
bsd build -o "$TEST_ROOT/result" "$flake1Dir?ref=HEAD#default"
bsd build -o "$TEST_ROOT/result" "git+file://$flake1Dir?ref=HEAD#default"

# Check that relative paths are allowed for git flakes.
# This may change in the future once git submodule support is refined.
# See: https://discourse.basedlinux.org/t/57783 and #9708.
(
  # This `cd` should not be required and is indicative of aforementioned bug.
  cd "$flake1Dir/.."
  bsd build -o "$TEST_ROOT/result" "git+file:./$(basename "$flake1Dir")"
)

# Check that store symlinks inside a flake are not interpreted as flakes.
bsd build -o "$flake1Dir/result" "git+file://$flake1Dir"
bsd path-info "$flake1Dir/result"

# 'getFlake' on an unlocked flakeref should fail in pure mode, but
# succeed in impure mode.
(! bsd build -o "$TEST_ROOT/result" --expr "(builtins.getFlake \"$flake1Dir\").packages.$system.default")
bsd build -o "$TEST_ROOT/result" --expr "(builtins.getFlake \"$flake1Dir\").packages.$system.default" --impure

# 'getFlake' on a locked flakeref should succeed even in pure mode.
bsd build -o "$TEST_ROOT/result" --expr "(builtins.getFlake \"git+file://$flake1Dir?rev=$hash2\").packages.$system.default"

# Regression test for dirOf on the root of the flake.
[[ $(bsd eval --json flake1#parent) = \""$NIX_STORE_DIR"\" ]]

# Regression test for baseNameOf on the root of the flake.
[[ $(bsd eval --raw flake1#baseName) =~ ^[a-z0-9]+-source$ ]]

# Test that the root of a tree returns a path named /bsd/store/<hash1>-<hash2>-source.
# This behavior is *not* desired, but has existed for a while.
# Issue #10627 what to do about it.
[[ $(bsd eval --raw flake1#root) =~ ^.*/[a-z0-9]+-[a-z0-9]+-source$ ]]

# Building a flake with an unlocked dependency should fail in pure mode.
(! bsd build -o "$TEST_ROOT/result" flake2#bar --no-registries)
(! bsd build -o "$TEST_ROOT/result" flake2#bar --no-use-registries)
(! bsd eval --expr "builtins.getFlake \"$flake2Dir\"")

# But should succeed in impure mode.
(! bsd build -o "$TEST_ROOT/result" flake2#bar --impure)
bsd build -o "$TEST_ROOT/result" flake2#bar --impure --no-write-lock-file
bsd eval --expr "builtins.getFlake \"$flake2Dir\"" --impure

# Building a local flake with an unlocked dependency should fail with --no-update-lock-file.
expect 1 bsd build -o "$TEST_ROOT/result" "$flake2Dir#bar" --no-update-lock-file 2>&1 | grep 'requires lock file changes'

# But it should succeed without that flag.
bsd build -o "$TEST_ROOT/result" "$flake2Dir#bar" --no-write-lock-file
expect 1 bsd build -o "$TEST_ROOT/result" "$flake2Dir#bar" --no-update-lock-file 2>&1 | grep 'requires lock file changes'
bsd build -o "$TEST_ROOT/result" "$flake2Dir#bar" --commit-lock-file
[[ -e "$flake2Dir/flake.lock" ]]
[[ -z $(git -C "$flake2Dir" diff main || echo failed) ]]
[[ $(jq --indent 0 --compact-output . < "$flake2Dir/flake.lock") =~ ^'{"nodes":{"flake1":{"locked":{"lastModified":'.*',"narHash":"sha256-'.*'","ref":"refs/heads/master","rev":"'.*'","revCount":2,"type":"git","url":"file:///'.*'"},"original":{"id":"flake1","type":"indirect"}},"root":{"inputs":{"flake1":"flake1"}}},"root":"root","version":7}'$ ]]

# Rerunning the build should not change the lockfile.
bsd build -o "$TEST_ROOT/result" "$flake2Dir#bar"
[[ -z $(git -C "$flake2Dir" diff main || echo failed) ]]

# Building with a lockfile should not require a fetch of the registry.
bsd build -o "$TEST_ROOT/result" --flake-registry file:///no-registry.json "$flake2Dir#bar" --refresh
bsd build -o "$TEST_ROOT/result" --no-registries "$flake2Dir#bar" --refresh
bsd build -o "$TEST_ROOT/result" --no-use-registries "$flake2Dir#bar" --refresh

# Updating the flake should not change the lockfile.
bsd flake lock "$flake2Dir"
[[ -z $(git -C "$flake2Dir" diff main || echo failed) ]]

# Now we should be able to build the flake in pure mode.
bsd build -o "$TEST_ROOT/result" flake2#bar

# Or without a registry.
bsd build -o "$TEST_ROOT/result" --no-registries "git+file://$percentEncodedFlake2Dir#bar" --refresh
bsd build -o "$TEST_ROOT/result" --no-use-registries "git+file://$percentEncodedFlake2Dir#bar" --refresh

# Test whether indirect dependencies work.
bsd build -o "$TEST_ROOT/result" "$flake3Dir#xyzzy"
git -C "$flake3Dir" add flake.lock

# Add dependency to flake3.
rm "$flake3Dir/flake.nix"

cat > "$flake3Dir/flake.nix" <<EOF
{
  description = "Fnord";

  outputs = { self, flake1, flake2 }: rec {
    packages.$system.xyzzy = flake2.packages.$system.bar;
    packages.$system."sth sth" = flake1.packages.$system.foo;
  };
}
EOF

git -C "$flake3Dir" add flake.nix
git -C "$flake3Dir" commit -m 'Update flake.nix'

# Check whether `bsd build` works with an incomplete lockfile
bsd build -o $TEST_ROOT/result "$flake3Dir#sth sth"
bsd build -o $TEST_ROOT/result "$flake3Dir#sth%20sth"

# Check whether it saved the lockfile
[[ -n $(git -C "$flake3Dir" diff master) ]]

git -C "$flake3Dir" add flake.lock

git -C "$flake3Dir" commit -m 'Add lockfile'

# Test whether registry caching works.
bsd registry list --flake-registry "file://$registry" | grepQuiet flake3
mv "$registry" "$registry.tmp"
bsd store gc
bsd registry list --flake-registry "file://$registry" --refresh | grepQuiet flake3
mv "$registry.tmp" "$registry"

# Ensure that locking ignores the user registry.
mkdir -p "$TEST_HOME/.config/bsd"
ln -sfn "$registry" "$TEST_HOME/.config/bsd/registry.json"
bsd flake metadata --flake-registry '' flake1
expectStderr 1 bsd flake update --flake-registry '' --flake "$flake3Dir" | grepQuiet "cannot find flake 'flake:flake1' in the flake registries"
rm "$TEST_HOME/.config/bsd/registry.json"

# Test whether flakes are registered as GC roots for offline use.
# FIXME: use tarballs rather than git.
rm -rf "$TEST_HOME/.cache"
bsd store gc # get rid of copies in the store to ensure they get fetched to our git cache
_NIX_FORCE_HTTP=1 bsd build -o "$TEST_ROOT/result" "git+file://$percentEncodedFlake2Dir#bar"
mv "$flake1Dir" "$flake1Dir.tmp"
mv "$flake2Dir" "$flake2Dir.tmp"
bsd store gc
_NIX_FORCE_HTTP=1 bsd build -o "$TEST_ROOT/result" "git+file://$percentEncodedFlake2Dir#bar"
_NIX_FORCE_HTTP=1 bsd build -o "$TEST_ROOT/result" "git+file://$percentEncodedFlake2Dir#bar" --refresh
mv "$flake1Dir.tmp" "$flake1Dir"
mv "$flake2Dir.tmp" "$flake2Dir"

# Test doing multiple `lookupFlake`s
bsd build -o "$TEST_ROOT/result" flake3#xyzzy

# Test 'bsd flake update' and --override-flake.
bsd flake lock "$flake3Dir"
[[ -z $(git -C "$flake3Dir" diff master || echo failed) ]]

bsd flake update --flake "$flake3Dir" --override-flake flake2 bsdpkgs
[[ ! -z $(git -C "$flake3Dir" diff master || echo failed) ]]

# Testing the bsd CLI
bsd registry add flake1 flake3
[[ $(bsd registry list | wc -l) == 5 ]]
bsd registry pin flake1
[[ $(bsd registry list | wc -l) == 5 ]]
bsd registry pin flake1 flake3
[[ $(bsd registry list | wc -l) == 5 ]]
bsd registry remove flake1
[[ $(bsd registry list | wc -l) == 4 ]]

# Test 'bsd registry list' with a disabled global registry.
bsd registry add user-flake1 git+file://$flake1Dir
bsd registry add user-flake2 "git+file://$percentEncodedFlake2Dir"
[[ $(bsd --flake-registry "" registry list | wc -l) == 2 ]]
bsd --flake-registry "" registry list | grepQuietInverse '^global' # nothing in global registry
bsd --flake-registry "" registry list | grepQuiet '^user'
bsd flake metadata --flake-registry "" user-flake1 | grepQuiet 'URL:.*flake1.*'
bsd registry remove user-flake1
bsd registry remove user-flake2
[[ $(bsd registry list | wc -l) == 4 ]]

# Test 'bsd flake clone'.
rm -rf $TEST_ROOT/flake1-v2
bsd flake clone flake1 --dest $TEST_ROOT/flake1-v2
[ -e $TEST_ROOT/flake1-v2/flake.nix ]

# Test 'follows' inputs.
cat > "$flake3Dir/flake.nix" <<EOF
{
  inputs.foo = {
    type = "indirect";
    id = "flake1";
  };
  inputs.bar.follows = "foo";

  outputs = { self, foo, bar }: {
  };
}
EOF

bsd flake lock "$flake3Dir"
[[ $(jq -c .nodes.root.inputs.bar "$flake3Dir/flake.lock") = '["foo"]' ]]

cat > "$flake3Dir/flake.nix" <<EOF
{
  inputs.bar.follows = "flake2/flake1";

  outputs = { self, flake2, bar }: {
  };
}
EOF

bsd flake lock "$flake3Dir"
[[ $(jq -c .nodes.root.inputs.bar "$flake3Dir/flake.lock") = '["flake2","flake1"]' ]]

cat > "$flake3Dir/flake.nix" <<EOF
{
  inputs.bar.follows = "flake2";

  outputs = { self, flake2, bar }: {
  };
}
EOF

bsd flake lock "$flake3Dir"
[[ $(jq -c .nodes.root.inputs.bar "$flake3Dir/flake.lock") = '["flake2"]' ]]

# Test overriding inputs of inputs.
writeTrivialFlake $flake7Dir
git -C $flake7Dir add flake.nix
git -C $flake7Dir commit -m 'Initial'

cat > "$flake3Dir/flake.nix" <<EOF
{
  inputs.flake2.inputs.flake1 = {
    type = "git";
    url = file://$flake7Dir;
  };

  outputs = { self, flake2 }: {
  };
}
EOF

bsd flake lock "$flake3Dir"
[[ $(jq .nodes.flake1.locked.url "$flake3Dir/flake.lock") =~ flake7 ]]

cat > "$flake3Dir/flake.nix" <<EOF
{
  inputs.flake2.inputs.flake1.follows = "foo";
  inputs.foo.url = git+file://$flake7Dir;

  outputs = { self, flake2 }: {
  };
}
EOF

bsd flake update --flake "$flake3Dir"
[[ $(jq -c .nodes.flake2.inputs.flake1 "$flake3Dir/flake.lock") =~ '["foo"]' ]]
[[ $(jq .nodes.foo.locked.url "$flake3Dir/flake.lock") =~ flake7 ]]

# Test git+file with bare repo.
rm -rf $flakeGitBare
git clone --bare $flake1Dir $flakeGitBare
bsd build -o $TEST_ROOT/result git+file://$flakeGitBare

# Test path flakes.
mkdir -p $flake5Dir
writeDependentFlake $flake5Dir
bsd flake lock path://$flake5Dir

# Test tarball flakes.
tar cfz $TEST_ROOT/flake.tar.gz -C $TEST_ROOT flake5

bsd build -o $TEST_ROOT/result file://$TEST_ROOT/flake.tar.gz

# Building with a tarball URL containing a SRI hash should also work.
url=$(bsd flake metadata --json file://$TEST_ROOT/flake.tar.gz | jq -r .url)
[[ $url =~ sha256- ]]

bsd build -o $TEST_ROOT/result $url

# Building with an incorrect SRI hash should fail.
expectStderr 102 bsd build -o $TEST_ROOT/result "file://$TEST_ROOT/flake.tar.gz?narHash=sha256-qQ2Zz4DNHViCUrp6gTS7EE4+RMqFQtUfWF2UNUtJKS0=" | grep 'NAR hash mismatch'

# Test --override-input.
git -C "$flake3Dir" reset --hard
bsd flake lock "$flake3Dir" --override-input flake2/flake1 file://$TEST_ROOT/flake.tar.gz -vvvvv
[[ $(jq .nodes.flake1_2.locked.url "$flake3Dir/flake.lock") =~ flake.tar.gz ]]

bsd flake lock "$flake3Dir" --override-input flake2/flake1 flake1
[[ $(jq -r .nodes.flake1_2.locked.rev "$flake3Dir/flake.lock") =~ $hash2 ]]

bsd flake lock "$flake3Dir" --override-input flake2/flake1 flake1/master/$hash1
[[ $(jq -r .nodes.flake1_2.locked.rev "$flake3Dir/flake.lock") =~ $hash1 ]]

# Test --update-input.
bsd flake lock "$flake3Dir"
[[ $(jq -r .nodes.flake1_2.locked.rev "$flake3Dir/flake.lock") = $hash1 ]]

bsd flake update flake2/flake1 --flake "$flake3Dir"
[[ $(jq -r .nodes.flake1_2.locked.rev "$flake3Dir/flake.lock") =~ $hash2 ]]

# Test updating multiple inputs.
bsd flake lock "$flake3Dir" --override-input flake1 flake1/master/$hash1
bsd flake lock "$flake3Dir" --override-input flake2/flake1 flake1/master/$hash1
[[ $(jq -r .nodes.flake1.locked.rev "$flake3Dir/flake.lock") =~ $hash1 ]]
[[ $(jq -r .nodes.flake1_2.locked.rev "$flake3Dir/flake.lock") =~ $hash1 ]]

bsd flake update flake1 flake2/flake1 --flake "$flake3Dir"
[[ $(jq -r .nodes.flake1.locked.rev "$flake3Dir/flake.lock") =~ $hash2 ]]
[[ $(jq -r .nodes.flake1_2.locked.rev "$flake3Dir/flake.lock") =~ $hash2 ]]

# Test 'bsd flake metadata --json'.
bsd flake metadata "$flake3Dir" --json | jq .

# Test flake in store does not evaluate.
rm -rf $badFlakeDir
mkdir $badFlakeDir
echo INVALID > $badFlakeDir/flake.nix
bsd store delete $(bsd store add-path $badFlakeDir)

[[ $(bsd path-info      $(bsd store add-path $flake1Dir)) =~ flake1 ]]
[[ $(bsd path-info path:$(bsd store add-path $flake1Dir)) =~ simple ]]

# Test fetching flakerefs in the legacy CLI.
[[ $(bsd-instantiate --eval flake:flake3 -A x) = 123 ]]
[[ $(bsd-instantiate --eval "flake:git+file://$percentEncodedFlake3Dir" -A x) = 123 ]]
[[ $(bsd-instantiate -I flake3=flake:flake3 --eval '<flake3>' -A x) = 123 ]]
[[ $(NIX_PATH=flake3=flake:flake3 bsd-instantiate --eval '<flake3>' -A x) = 123 ]]

# Test alternate lockfile paths.
bsd flake lock "$flake2Dir" --output-lock-file $TEST_ROOT/flake2.lock
cmp "$flake2Dir/flake.lock" $TEST_ROOT/flake2.lock >/dev/null # lockfiles should be identical, since we're referencing flake2's original one

bsd flake lock "$flake2Dir" --output-lock-file $TEST_ROOT/flake2-overridden.lock --override-input flake1 git+file://$flake1Dir?rev=$flake1OriginalCommit
expectStderr 1 cmp "$flake2Dir/flake.lock" $TEST_ROOT/flake2-overridden.lock
bsd flake metadata "$flake2Dir" --reference-lock-file $TEST_ROOT/flake2-overridden.lock | grepQuiet $flake1OriginalCommit

# reference-lock-file can only be used if allow-dirty is set.
expectStderr 1 bsd flake metadata "$flake2Dir" --no-allow-dirty --reference-lock-file $TEST_ROOT/flake2-overridden.lock
