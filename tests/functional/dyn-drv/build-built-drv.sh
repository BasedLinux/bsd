#!/usr/bin/env bash

source common.sh

# In the corresponding bsd file, we have two derivations: the first, named `hello`,
# is a normal recursive derivation, while the second, named dependent, has the
# new outputHashMode "text". Note that in "dependent", we don't refer to the
# build output of `hello`, but only to the path of the drv file. For this reason,
# we only need to:
#
# - instantiate `hello`
# - build `producingDrv`
# - check that the path of the output coincides with that of the original derivation

out1=$(bsd build -f ./text-hashed-output.bsd hello --no-link)

clearStore

drvDep=$(bsd-instantiate ./text-hashed-output.bsd -A producingDrv)

# Store layer needs bugfix
requireDaemonNewerThan "2.30pre20250515"

out2=$(bsd build "${drvDep}^out^out" --no-link)

test $out1 == $out2
