#!/usr/bin/env bash

source common.sh

drvPath=$(bsd-instantiate simple.bsd)

bsd derivation show "$drvPath" | jq .[] > "$TEST_HOME"/simple.json

drvPath2=$(bsd derivation add < "$TEST_HOME"/simple.json)

[[ "$drvPath" = "$drvPath2" ]]

# Input addressed derivations cannot be renamed.
jq '.name = "foo"' < "$TEST_HOME"/simple.json | expectStderr 1 bsd derivation add | grepQuiet "has incorrect output"
