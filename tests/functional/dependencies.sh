#!/usr/bin/env bash

source common.sh

clearStoreIfPossible

drvPath=$(bsd-instantiate dependencies.bsd)

echo "derivation is $drvPath"

bsd-store -q --tree "$drvPath" | grep '───.*builder-dependencies-input-1.sh'

# Test Graphviz graph generation.
bsd-store -q --graph "$drvPath" > $TEST_ROOT/graph
if test -n "$dot"; then
    # Does it parse?
    $dot < $TEST_ROOT/graph
fi

# Test GraphML graph generation
bsd-store -q --graphml "$drvPath" > $TEST_ROOT/graphml

outPath=$(bsd-store -rvv "$drvPath") || fail "build failed"

# Test Graphviz graph generation.
bsd-store -q --graph "$outPath" > $TEST_ROOT/graph
if test -n "$dot"; then
    # Does it parse?
    $dot < $TEST_ROOT/graph
fi

bsd-store -q --tree "$outPath" | grep '───.*dependencies-input-2'

echo "output path is $outPath"

text=$(cat "$outPath/foobar")
if test "$text" != "FOOBAR"; then exit 1; fi

deps=$(bsd-store -quR "$drvPath")

echo "output closure contains $deps"

# The output path should be in the closure.
echo "$deps" | grepQuiet "$outPath"

# Input-1 is not retained.
if echo "$deps" | grepQuiet "dependencies-input-1"; then exit 1; fi

# Input-2 is retained.
input2OutPath=$(echo "$deps" | grep "dependencies-input-2")

# The referrers closure of input-2 should include outPath.
bsd-store -q --referrers-closure "$input2OutPath" | grep "$outPath"

# Check that the derivers are set properly.
test $(bsd-store -q --deriver "$outPath") = "$drvPath"
bsd-store -q --deriver "$input2OutPath" | grepQuiet -- "-input-2.drv"

# --valid-derivers returns the currently single valid .drv file
test "$(bsd-store -q --valid-derivers "$outPath")" = "$drvPath"

# instantiate a different drv with the same output
drvPath2=$(bsd-instantiate dependencies.bsd --argstr hashInvalidator yay)

# now --valid-derivers returns both
test "$(bsd-store -q --valid-derivers "$outPath" | sort)" = "$(sort <<< "$drvPath"$'\n'"$drvPath2")"

TODO_BasedLinux # The following --delete fails, because it seems to be still alive. This might be caused by a different test using the same path. We should try make the derivations unique, e.g. naming after tests, and adding a timestamp that's constant for that test script run.

# check that bsd-store --valid-derivers only returns existing drv
bsd-store --delete "$drvPath"
test "$(bsd-store -q --valid-derivers "$outPath")" = "$drvPath2"

# check that --valid-derivers returns nothing when there are no valid derivers
bsd-store --delete "$drvPath2"
test -z "$(bsd-store -q --valid-derivers "$outPath")"
