#!/usr/bin/env bash

source common.sh

TODO_BasedLinux

clearStore

path=$(bsd-store -q $(bsd-instantiate fixed.bsd -A good.0))

echo 'testing bad...'
bsd-build fixed.bsd -A bad --no-out-link && fail "should fail"

# Building with the bad hash should produce the "good" output path as
# a side-effect.
[[ -e $path ]]
bsd path-info --json $path | grep fixed:md5:2qk15sxzzjlnpjk9brn7j8ppcd

echo 'testing good...'
bsd-build fixed.bsd -A good --no-out-link

if isDaemonNewer "2.4pre20210927"; then
    echo 'testing --check...'
    bsd-build fixed.bsd -A check --check && fail "should fail"
fi

echo 'testing good2...'
bsd-build fixed.bsd -A good2 --no-out-link

echo 'testing reallyBad...'
bsd-instantiate fixed.bsd -A reallyBad && fail "should fail"

if isDaemonNewer "2.20pre20240108"; then
    echo 'testing fixed with references...'
    expectStderr 1 bsd-build fixed.bsd -A badReferences | grepQuiet "not allowed to refer to other store paths"
fi

# While we're at it, check attribute selection a bit more.
echo 'testing attribute selection...'
test $(bsd-instantiate fixed.bsd -A good.1 | wc -l) = 1

# Test parallel builds of derivations that produce the same output.
# Only one should run at the same time.
echo 'testing parallelSame...'
clearStore
bsd-build fixed.bsd -A parallelSame --no-out-link -j2

# Fixed-output derivations with a recursive SHA-256 hash should
# produce the same path as "bsd-store --add".
echo 'testing sameAsAdd...'
out=$(bsd-build fixed.bsd -A sameAsAdd --no-out-link)

# This is what fixed.builder2 produces...
rm -rf $TEST_ROOT/fixed
mkdir $TEST_ROOT/fixed
mkdir $TEST_ROOT/fixed/bla
echo "Hello World!" > $TEST_ROOT/fixed/foo
ln -s foo $TEST_ROOT/fixed/bar

out2=$(bsd-store --add $TEST_ROOT/fixed)
[ "$out" = "$out2" ]

out3=$(bsd-store --add-fixed --recursive sha256 $TEST_ROOT/fixed)
[ "$out" = "$out3" ]

out4=$(bsd-store --print-fixed-path --recursive sha256 "1ixr6yd3297ciyp9im522dfxpqbkhcw0pylkb2aab915278fqaik" fixed)
[ "$out" = "$out4" ]

# Can use `outputHashMode = "nar";` instead of `"recursive"` now.
clearStore
bsd-build fixed.bsd -A nar-not-recursive --no-out-link
