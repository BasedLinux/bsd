#!/usr/bin/env bash

source common.sh

# Store layer needs bugfix
requireDaemonNewerThan "2.30pre20250515"

TODO_BasedLinux # can't enable a sandbox feature easily

enableFeatures 'recursive-bsd'
restartDaemon

NIX_BIN_DIR="$(dirname "$(type -p bsd)")"
export NIX_BIN_DIR

bsd build -L --file ./non-trivial.bsd --no-link
