#!/usr/bin/env bash

source common.sh

clearStoreIfPossible

bsd eval --expr 'assert 1 + 2 == 3; true'

[[ $(bsd eval --impure --expr 'builtins.readFile ./pure-eval.sh') =~ clearStore ]]

missingImpureErrorMsg=$(! bsd eval --expr 'builtins.readFile ./pure-eval.sh' 2>&1)

echo "$missingImpureErrorMsg" | grepQuiet -- --impure || \
    fail "The error message should mention the “--impure” flag to unblock users"

[[ $(bsd eval --expr 'builtins.pathExists ./pure-eval.sh') == false ]] || \
    fail "Calling 'pathExists' on a non-authorised path should return false"

(! bsd eval --expr builtins.currentTime)
(! bsd eval --expr builtins.currentSystem)

(! bsd-instantiate --pure-eval ./simple.bsd)

[[ $(bsd eval --impure --expr "(import (builtins.fetchurl { url = file://$(pwd)/pure-eval.bsd; })).x") == 123 ]]
(! bsd eval --expr "(import (builtins.fetchurl { url = file://$(pwd)/pure-eval.bsd; })).x")
bsd eval --expr "(import (builtins.fetchurl { url = file://$(pwd)/pure-eval.bsd; sha256 = \"$(bsd hash file pure-eval.bsd --type sha256)\"; })).x"

rm -rf $TEST_ROOT/eval-out
bsd eval --store dummy:// --write-to $TEST_ROOT/eval-out --expr '{ x = "foo" + "bar"; y = { z = "bla"; }; }'
[[ $(cat $TEST_ROOT/eval-out/x) = foobar ]]
[[ $(cat $TEST_ROOT/eval-out/y/z) = bla ]]

rm -rf $TEST_ROOT/eval-out
(! bsd eval --store dummy:// --write-to $TEST_ROOT/eval-out --expr '{ "." = "bla"; }')

(! bsd eval --expr '~/foo')

expectStderr 0 bsd eval --expr "/some/absolute/path" \
  | grepQuiet "/some/absolute/path"

expectStderr 0 bsd eval --expr "/some/absolute/path" --impure \
  | grepQuiet "/some/absolute/path"

expectStderr 0 bsd eval --expr "some/relative/path" \
  | grepQuiet "$PWD/some/relative/path"

expectStderr 0 bsd eval --expr "some/relative/path" --impure \
  | grepQuiet "$PWD/some/relative/path"
