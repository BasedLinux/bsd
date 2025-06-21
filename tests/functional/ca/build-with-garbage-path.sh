#!/usr/bin/env bash

# Regression test for https://github.com/BasedLinux/bsd/issues/4858

source common.sh

requireDaemonNewerThan "2.4pre20210621"

# Get the output path of `rootCA`, and put some garbage instead
outPath="$(bsd-build ./content-addressed.bsd -A rootCA --no-out-link)"
bsd-store --delete $(bsd-store -q --referrers-closure "$outPath")
touch "$outPath"

# The build should correctly remove the garbage and put the expected path instead
bsd-build ./content-addressed.bsd -A rootCA --no-out-link

# Rebuild it. This shouldn’t overwrite the existing path
oldInode=$(stat -c '%i' "$outPath")
bsd-build ./content-addressed.bsd -A rootCA --no-out-link --arg seed 2
newInode=$(stat -c '%i' "$outPath")
[[ "$oldInode" == "$newInode" ]]
