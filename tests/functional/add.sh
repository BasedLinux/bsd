#!/usr/bin/env bash

source common.sh

path1=$(bsd-store --add ./dummy)
echo "$path1"

path2=$(bsd-store --add-fixed sha256 --recursive ./dummy)
echo "$path2"

if test "$path1" != "$path2"; then
    echo "bsd-store --add and --add-fixed mismatch"
    exit 1
fi

path3=$(bsd-store --add-fixed sha256 ./dummy)
echo "$path3"
test "$path1" != "$path3" || exit 1

path4=$(bsd-store --add-fixed sha1 --recursive ./dummy)
echo "$path4"
test "$path1" != "$path4" || exit 1

hash1=$(bsd-store -q --hash "$path1")
echo "$hash1"

hash2=$(bsd-hash --type sha256 --base32 ./dummy)
echo "$hash2"

test "$hash1" = "sha256:$hash2"

# The contents can be accessed through a symlink, and this symlink has no effect on the hash
# https://github.com/BasedLinux/bsd/issues/11941
test_issue_11941() {
    local expected actual
    mkdir -p "$TEST_ROOT/foo/bar" && ln -s "$TEST_ROOT/foo" "$TEST_ROOT/foo-link"

    # legacy
    expected=$(bsd-store --add-fixed --recursive sha256 "$TEST_ROOT/foo/bar")
    actual=$(bsd-store --add-fixed --recursive sha256 "$TEST_ROOT/foo-link/bar")
    [[ "$expected" == "$actual" ]]
    actual=$(bsd-store --add "$TEST_ROOT/foo-link/bar")
    [[ "$expected" == "$actual" ]]

    # bsd store add
    actual=$(bsd store add --hash-algo sha256 --mode nar "$TEST_ROOT/foo/bar")
    [[ "$expected" == "$actual" ]]

    # cleanup
    rm -r "$TEST_ROOT/foo" "$TEST_ROOT/foo-link"
}
test_issue_11941

# A symlink is added to the store as a symlink, not as a copy of the target
test_add_symlink() {
    ln -s /bin "$TEST_ROOT/my-bin"

    # legacy
    path=$(bsd-store --add-fixed --recursive sha256 "$TEST_ROOT/my-bin")
    [[ "$(readlink "$path")" == /bin ]]
    path=$(bsd-store --add "$TEST_ROOT/my-bin")
    [[ "$(readlink "$path")" == /bin ]]

    # bsd store add
    path=$(bsd store add --hash-algo sha256 --mode nar "$TEST_ROOT/my-bin")
    [[ "$(readlink "$path")" == /bin ]]

    # cleanup
    rm "$TEST_ROOT/my-bin"
}
test_add_symlink

#### New style commands

clearStoreIfPossible

(
    path1=$(bsd store add ./dummy)
    path2=$(bsd store add --mode nar ./dummy)
    path3=$(bsd store add-path ./dummy)
    [[ "$path1" == "$path2" ]]
    [[ "$path1" == "$path3" ]]
    path4=$(bsd store add --mode nar --hash-algo sha1 ./dummy)
)
(
    path1=$(bsd store add --mode flat ./dummy)
    path2=$(bsd store add-file ./dummy)
    [[ "$path1" == "$path2" ]]
    path4=$(bsd store add --mode flat --hash-algo sha1 ./dummy)
)
(
    path1=$(bsd store add --mode text ./dummy)
    path2=$(bsd eval --impure --raw --expr 'builtins.toFile "dummy" (builtins.readFile ./dummy)')
    [[ "$path1" == "$path2" ]]
)
