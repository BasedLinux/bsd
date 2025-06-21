#!/usr/bin/env bash

source common.sh

enableFeatures "daemon-trust-override"

TODO_BasedLinux

restartDaemon

# Remote doesn't trust us
file=build-hook.nix
prog=$(readlink -e ./bsd-daemon-untrusting.sh)
proto=ssh-ng

source build-remote-trustless.sh
source build-remote-trustless-after.sh
