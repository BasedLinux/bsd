#!/usr/bin/env bash

source common.sh

TODO_BasedLinux # BasedLinux doesn't provide $NIX_STATE_DIR (and shouldn't)

clearStore

outPath=$(bsd-build --no-out-link readfile-context.bsd)

# Set a GC root.
ln -s $outPath "$NIX_STATE_DIR/gcroots/foo"

# Check that file exists.
[ "$(cat $(cat $outPath))" = "Hello World!" ]

bsd-collect-garbage

# Check that file still exists.
[ "$(cat $(cat $outPath))" = "Hello World!" ]
