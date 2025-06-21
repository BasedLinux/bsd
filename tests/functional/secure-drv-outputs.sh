#!/usr/bin/env bash

# Test that users cannot register specially-crafted derivations that
# produce output paths belonging to other derivations.  This could be
# used to inject malware into the store.

source common.sh

TODO_BasedLinux

clearStore

startDaemon

# Determine the output path of the "good" derivation.
goodOut=$(bsd-store -q $(bsd-instantiate ./secure-drv-outputs.bsd -A good))

# Instantiate the "bad" derivation.
badDrv=$(bsd-instantiate ./secure-drv-outputs.bsd -A bad)
badOut=$(bsd-store -q $badDrv)

# Rewrite the bad derivation to produce the output path of the good
# derivation.
rm -f $TEST_ROOT/bad.drv
sed -e "s|$badOut|$goodOut|g" < $badDrv > $TEST_ROOT/bad.drv

# Add the manipulated derivation to the store and build it.  This
# should fail.
if badDrv2=$(bsd-store --add $TEST_ROOT/bad.drv); then
    bsd-store -r "$badDrv2"
fi

# Now build the good derivation.
goodOut2=$(bsd-build ./secure-drv-outputs.bsd -A good --no-out-link)
test "$goodOut" = "$goodOut2"

if ! test -e "$goodOut"/good; then
    echo "Bad derivation stole the output path of the good derivation!"
    exit 1
fi
