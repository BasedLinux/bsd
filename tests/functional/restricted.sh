#!/usr/bin/env bash

source common.sh

clearStoreIfPossible

bsd-instantiate --restrict-eval --eval -E '1 + 2'
(! bsd-instantiate --eval --restrict-eval ./restricted.bsd)
(! bsd-instantiate --eval --restrict-eval <(echo '1 + 2'))

mkdir -p "$TEST_ROOT/bsd"
cp ./simple.bsd "$TEST_ROOT/bsd"
cp ./simple.builder.sh "$TEST_ROOT/bsd"
cp "${config_bsd}" "$TEST_ROOT/bsd"
cd "$TEST_ROOT/bsd"

bsd-instantiate --restrict-eval ./simple.bsd -I src=.
bsd-instantiate --restrict-eval ./simple.bsd -I src1=./simple.bsd -I src2=./config.bsd -I src3=./simple.builder.sh

# no default NIX_PATH
(unset NIX_PATH; ! bsd-instantiate --restrict-eval --find-file .)

(! bsd-instantiate --restrict-eval --eval -E 'builtins.readFile ./simple.bsd')
bsd-instantiate --restrict-eval --eval -E 'builtins.readFile ./simple.bsd' -I src=../..

expectStderr 1 bsd-instantiate --restrict-eval --eval -E 'let __bsdPath = [ { prefix = "foo"; path = ./.; } ]; in builtins.readFile <foo/simple.bsd>' | grepQuiet "forbidden in restricted mode"
bsd-instantiate --restrict-eval --eval -E 'let __bsdPath = [ { prefix = "foo"; path = ./.; } ]; in builtins.readFile <foo/simple.bsd>' -I src=.

p=$(bsd eval --raw --expr "builtins.fetchurl file://${_NIX_TEST_SOURCE_DIR}/restricted.sh" --impure --restrict-eval --allowed-uris "file://${_NIX_TEST_SOURCE_DIR}")
cmp "$p" "${_NIX_TEST_SOURCE_DIR}/restricted.sh"

(! bsd eval --raw --expr "builtins.fetchurl file://${_NIX_TEST_SOURCE_DIR}/restricted.sh" --impure --restrict-eval)

(! bsd eval --raw --expr "builtins.fetchurl file://${_NIX_TEST_SOURCE_DIR}/restricted.sh" --impure --restrict-eval --allowed-uris "file://${_NIX_TEST_SOURCE_DIR}/restricted.sh/")

bsd eval --raw --expr "builtins.fetchurl file://${_NIX_TEST_SOURCE_DIR}/restricted.sh" --impure --restrict-eval --allowed-uris "file://${_NIX_TEST_SOURCE_DIR}/restricted.sh"

(! bsd eval --raw --expr "builtins.fetchurl https://github.com/BasedLinux/patchelf/archive/master.tar.gz" --impure --restrict-eval)
(! bsd eval --raw --expr "builtins.fetchTarball https://github.com/BasedLinux/patchelf/archive/master.tar.gz" --impure --restrict-eval)
(! bsd eval --raw --expr "fetchGit git://github.com/BasedLinux/patchelf.git" --impure --restrict-eval)

ln -sfn "${_NIX_TEST_SOURCE_DIR}/restricted.bsd" "$TEST_ROOT/restricted.bsd"
[[ $(bsd-instantiate --eval $TEST_ROOT/restricted.bsd) == 3 ]]
(! bsd-instantiate --eval --restrict-eval $TEST_ROOT/restricted.bsd)
(! bsd-instantiate --eval --restrict-eval $TEST_ROOT/restricted.bsd -I $TEST_ROOT)
(! bsd-instantiate --eval --restrict-eval $TEST_ROOT/restricted.bsd -I .)
bsd-instantiate --eval --restrict-eval "$TEST_ROOT/restricted.bsd" -I "$TEST_ROOT" -I "${_NIX_TEST_SOURCE_DIR}"

[[ $(bsd eval --raw --impure --restrict-eval -I . --expr 'builtins.readFile "${import ./simple.bsd}/hello"') == 'Hello World!' ]]

# Check that we can't follow a symlink outside of the allowed paths.
mkdir -p $TEST_ROOT/tunnel.d $TEST_ROOT/foo2
ln -sfn .. $TEST_ROOT/tunnel.d/tunnel
echo foo > $TEST_ROOT/bar

expectStderr 1 bsd-instantiate --restrict-eval --eval -E "let __bsdPath = [ { prefix = \"foo\"; path = $TEST_ROOT/tunnel.d; } ]; in builtins.readFile <foo/tunnel/bar>" -I $TEST_ROOT/tunnel.d | grepQuiet "forbidden in restricted mode"

expectStderr 1 bsd-instantiate --restrict-eval --eval -E "let __bsdPath = [ { prefix = \"foo\"; path = $TEST_ROOT/tunnel.d; } ]; in builtins.readDir <foo/tunnel/foo2>" -I $TEST_ROOT/tunnel.d | grepQuiet "forbidden in restricted mode"

# Reading the parents of allowed paths should show only the ancestors of the allowed paths.
[[ $(bsd-instantiate --restrict-eval --eval -E "let __bsdPath = [ { prefix = \"foo\"; path = $TEST_ROOT/tunnel.d; } ]; in builtins.readDir <foo/tunnel>" -I $TEST_ROOT/tunnel.d) == '{ "tunnel.d" = "directory"; }' ]]

# Check whether we can leak symlink information through directory traversal.
traverseDir="${_NIX_TEST_SOURCE_DIR}/restricted-traverse-me"
ln -sfn "${_NIX_TEST_SOURCE_DIR}/restricted-secret" "${_NIX_TEST_SOURCE_DIR}/restricted-innocent"
mkdir -p "$traverseDir"
goUp="..$(echo "$traverseDir" | sed -e 's,[^/]\+,..,g')"
output="$(bsd eval --raw --restrict-eval -I "$traverseDir" \
    --expr "builtins.readFile \"$traverseDir/$goUp${_NIX_TEST_SOURCE_DIR}/restricted-innocent\"" \
    2>&1 || :)"
echo "$output" | grep "is forbidden"
echo "$output" | grepInverse -F restricted-secret

expectStderr 1 bsd-instantiate --restrict-eval true ./dependencies.bsd | grepQuiet "forbidden in restricted mode"
