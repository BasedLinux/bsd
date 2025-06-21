#!/usr/bin/env bash

source common.sh

TODO_BasedLinux

clearStoreIfPossible

outPath=$(bsd-build dependencies.bsd -o $TEST_ROOT/result)
test "$(cat $TEST_ROOT/result/foobar)" = FOOBAR

# The result should be retained by a GC.
echo A
target=$(readLink $TEST_ROOT/result)
echo B
echo target is $target
bsd-store --gc
test -e $target/foobar

# But now it should be gone.
rm $TEST_ROOT/result
bsd-store --gc
if test -e $target/foobar; then false; fi

outPath2=$(bsd-build $(bsd-instantiate dependencies.bsd) --no-out-link)
[[ $outPath = $outPath2 ]]

outPath2=$(bsd-build $(bsd-instantiate dependencies.bsd)!out --no-out-link)
[[ $outPath = $outPath2 ]]

outPath2=$(bsd-store -r $(bsd-instantiate --add-root $TEST_ROOT/indirect dependencies.bsd)!out)
[[ $outPath = $outPath2 ]]

# The order of the paths on stdout must correspond to the -A options
# https://github.com/BasedLinux/bsd/issues/4197

input0="$(bsd-build bsd-build-examples.bsd -A input0 --no-out-link)"
input1="$(bsd-build bsd-build-examples.bsd -A input1 --no-out-link)"
input2="$(bsd-build bsd-build-examples.bsd -A input2 --no-out-link)"
body="$(bsd-build bsd-build-examples.bsd -A body --no-out-link)"

outPathsA="$(echo $(bsd-build bsd-build-examples.bsd -A input0 -A input1 -A input2 -A body --no-out-link))"
[[ "$outPathsA" = "$input0 $input1 $input2 $body" ]]

# test a different ordering to make sure it fails, not just in 23 out of 24 permutations
outPathsB="$(echo $(bsd-build bsd-build-examples.bsd -A body -A input1 -A input2 -A input0 --no-out-link))"
[[ "$outPathsB" = "$body $input1 $input2 $input0" ]]
