#!/usr/bin/env bash

source common.sh

# Remote trusts us
file=build-hook.bsd
prog=bsd-daemon
proto=ssh-ng

source build-remote-trustless.sh
source build-remote-trustless-after.sh
