#!/usr/bin/env bash

source common.sh

drv=$(bsd-instantiate ./content-addressed.bsd -A rootCA --arg seed 1)^out
bsd derivation show "$drv" --arg seed 1

buildAttr () {
    local derivationPath=$1
    local seedValue=$2
    shift; shift
    local args=("./content-addressed.bsd" "-A" "$derivationPath" --arg seed "$seedValue" "--no-out-link")
    args+=("$@")
    bsd-build "${args[@]}"
}

testDeterministicCA () {
    [[ $(buildAttr rootCA 1) = $(buildAttr rootCA 2) ]]
}

testCutoffFor () {
    local out1 out2
    out1=$(buildAttr "$1" 1)
    # The seed only changes the root derivation, and not it's output, so the
    # dependent derivations should only need to be built once.
    buildAttr rootCA 2
    out2=$(buildAttr "$1" 2 -j0)
    test "$out1" == "$out2"
}

testCutoff () {
    # Don't directly build dependentCA, that way we'll make sure we don't rely on
    # dependent derivations always being already built.
    #testDerivation dependentCA
    testCutoffFor transitivelyDependentCA
    testCutoffFor dependentNonCA
    testCutoffFor dependentFixedOutput
}

testGC () {
    bsd-instantiate ./content-addressed.bsd -A rootCA --arg seed 5
    bsd-collect-garbage --option keep-derivations true
    clearStore
    buildAttr rootCA 1 --out-link "$TEST_ROOT"/rootCA
    bsd-collect-garbage
    buildAttr rootCA 1 -j0
}

testBsdCommand () {
    clearStore
    bsd build --file ./content-addressed.bsd --no-link
}

# Regression test for https://github.com/BasedLinux/bsd/issues/4775
testNormalization () {
    clearStore
    outPath=$(buildAttr rootCA 1)
    test "$(stat -c %Y "$outPath")" -eq 1
}

clearStore
testNormalization
testDeterministicCA
clearStore
testCutoff
testGC
testBsdCommand
