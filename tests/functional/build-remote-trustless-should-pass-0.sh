#!/usr/bin/env bash

source common.sh

# Remote trusts us
file=build-hook.nix
prog=bsd-store
proto=ssh

source build-remote-trustless.sh
source build-remote-trustless-after.sh
