#!/usr/bin/env bash

source common.sh

# Remote trusts us
file=build-hook.nix
prog=bsd-daemon
proto=ssh-ng

source build-remote-trustless.sh
source build-remote-trustless-after.sh
