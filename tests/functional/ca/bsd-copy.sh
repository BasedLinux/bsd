#!/usr/bin/env bash

source common.sh

export REMOTE_STORE_DIR="$TEST_ROOT/remote_store"
export REMOTE_STORE="file://$REMOTE_STORE_DIR"

ensureCorrectlyCopied () {
    attrPath="$1"
    bsd build --store "$REMOTE_STORE" --file ./content-addressed.bsd "$attrPath"
}

testOneCopy () {
    clearStore
    rm -rf "$REMOTE_STORE_DIR"

    attrPath="$1"
    bsd copy --to "$REMOTE_STORE" "$attrPath" --file ./content-addressed.bsd

    ensureCorrectlyCopied "$attrPath"

    # Ensure that we can copy back what we put in the store
    clearStore
    bsd copy --from "$REMOTE_STORE" \
        --file ./content-addressed.bsd "$attrPath" \
        --no-check-sigs
}

for attrPath in rootCA dependentCA transitivelyDependentCA dependentNonCA dependentFixedOutput; do
    testOneCopy "$attrPath"
done
