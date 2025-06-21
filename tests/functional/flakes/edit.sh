#!/usr/bin/env bash

source ./common.sh

createFlake1

export EDITOR=cat
bsd edit "$flake1Dir#" | grepQuiet simple.builder.sh
