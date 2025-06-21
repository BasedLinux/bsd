#!/usr/bin/env bash

source common.sh

enableFeatures "daemon-trust-override"

TODO_BasedLinux
restartDaemon

# Remote doesn't trusts us, but this is fine because we are only
# building (fixed) CA derivations.
file=build-hook-ca-fixed.bsd
prog=$(readlink -e ./bsd-daemon-untrusting.sh)
proto=ssh-ng

source build-remote-trustless.sh
source build-remote-trustless-after.sh
