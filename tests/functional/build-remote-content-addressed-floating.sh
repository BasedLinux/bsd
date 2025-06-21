#!/usr/bin/env bash

source common.sh

file=build-hook-ca-floating.bsd

enableFeatures "ca-derivations"

NIX_TESTS_CA_BY_DEFAULT=true

source build-remote.sh
