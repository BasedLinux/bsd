#!/usr/bin/env bash

source common.sh

TODO_BasedLinux

clearStoreIfPossible

rm -f $TEST_ROOT/result*

# Test whether the output names match our expectations
outPath=$(bsd-instantiate multiple-outputs.bsd --eval -A nameCheck.out.outPath)
[ "$(echo "$outPath" | sed -E 's_^".*/[^-/]*-([^/]*)"$_\1_')" = "multiple-outputs-a" ]
outPath=$(bsd-instantiate multiple-outputs.bsd --eval -A nameCheck.dev.outPath)
[ "$(echo "$outPath" | sed -E 's_^".*/[^-/]*-([^/]*)"$_\1_')" = "multiple-outputs-a-dev" ]

# Test whether read-only evaluation works when referring to the
# ‘drvPath’ attribute.
echo "evaluating c..."
#drvPath=$(bsd-instantiate multiple-outputs.bsd -A c --readonly-mode)

# And check whether the resulting derivation explicitly depends on all
# outputs.
drvPath=$(bsd-instantiate multiple-outputs.bsd -A c)
#[ "$drvPath" = "$drvPath2" ]
grepQuiet 'multiple-outputs-a.drv",\["first","second"\]' $drvPath
grepQuiet 'multiple-outputs-b.drv",\["out"\]' $drvPath

# While we're at it, test the ‘unsafeDiscardOutputDependency’ primop.
outPath=$(bsd-build multiple-outputs.bsd -A d --no-out-link)
drvPath=$(cat $outPath/drv)
outPath=$(bsd-store -q $drvPath)
(! [ -e "$outPath" ])

# Do a build of something that depends on a derivation with multiple
# outputs.
echo "building b..."
outPath=$(bsd-build multiple-outputs.bsd -A b --no-out-link)
echo "output path is $outPath"
[ "$(cat "$outPath/file")" = "success" ]

# Test bsd-build on a derivation with multiple outputs.
outPath1=$(bsd-build multiple-outputs.bsd -A a -o $TEST_ROOT/result)
[ -e $TEST_ROOT/result-first ]
(! [ -e $TEST_ROOT/result-second ])
bsd-build multiple-outputs.bsd -A a.all -o $TEST_ROOT/result
[ "$(cat $TEST_ROOT/result-first/file)" = "first" ]
[ "$(cat $TEST_ROOT/result-second/file)" = "second" ]
[ "$(cat $TEST_ROOT/result-second/link/file)" = "first" ]
hash1=$(bsd-store -q --hash $TEST_ROOT/result-second)

outPath2=$(bsd-build $(bsd-instantiate multiple-outputs.bsd -A a) --no-out-link)
[[ $outPath1 = $outPath2 ]]

outPath2=$(bsd-build $(bsd-instantiate multiple-outputs.bsd -A a.first) --no-out-link)
[[ $outPath1 = $outPath2 ]]

outPath2=$(bsd-build $(bsd-instantiate multiple-outputs.bsd -A a.second) --no-out-link)
[[ $(cat $outPath2/file) = second ]]

[[ $(bsd-build $(bsd-instantiate multiple-outputs.bsd -A a.all) --no-out-link | wc -l) -eq 2 ]]

# Delete one of the outputs and rebuild it.  This will cause a hash
# rewrite.
env -u NIX_REMOTE bsd store delete $TEST_ROOT/result-second --ignore-liveness
bsd-build multiple-outputs.bsd -A a.all -o $TEST_ROOT/result
[ "$(cat $TEST_ROOT/result-second/file)" = "second" ]
[ "$(cat $TEST_ROOT/result-second/link/file)" = "first" ]
hash2=$(bsd-store -q --hash $TEST_ROOT/result-second)
[ "$hash1" = "$hash2" ]

# Make sure that bsd-build works on derivations with multiple outputs.
echo "building a.first..."
bsd-build multiple-outputs.bsd -A a.first --no-out-link

# Cyclic outputs should be rejected.
echo "building cyclic..."
if bsd-build multiple-outputs.bsd -A cyclic --no-out-link; then
    echo "Cyclic outputs incorrectly accepted!"
    exit 1
fi

# Do a GC. This should leave an empty store.
echo "collecting garbage..."
rm $TEST_ROOT/result*
bsd-store --gc --keep-derivations --keep-outputs
bsd-store --gc --print-roots
rm -rf $NIX_STORE_DIR/.links
rmdir $NIX_STORE_DIR

expect 1 bsd build -f multiple-outputs.bsd invalid-output-name-1 2>&1 | grep 'contains illegal character'
expect 1 bsd build -f multiple-outputs.bsd invalid-output-name-2 2>&1 | grep 'contains illegal character'
