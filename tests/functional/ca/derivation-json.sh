#!/usr/bin/env bash

source common.sh

export NIX_TESTS_CA_BY_DEFAULT=1

drvPath=$(bsd-instantiate ../simple.nix)

bsd derivation show "$drvPath" | jq .[] > "$TEST_HOME"/simple.json

drvPath2=$(bsd derivation add < "$TEST_HOME"/simple.json)

[[ "$drvPath" = "$drvPath2" ]]

# Content-addressing derivations can be renamed.
jq '.name = "foo"' < "$TEST_HOME"/simple.json > "$TEST_HOME"/foo.json
drvPath3=$(bsd derivation add --dry-run < "$TEST_HOME"/foo.json)
# With --dry-run nothing is actually written
[[ ! -e "$drvPath3" ]]

# But the JSON is rejected without the experimental feature
expectStderr 1 bsd derivation add < "$TEST_HOME"/foo.json --experimental-features bsd-command | grepQuiet "experimental Bsd feature 'ca-derivations' is disabled"

# Without --dry-run it is actually written
drvPath4=$(bsd derivation add < "$TEST_HOME"/foo.json)
[[ "$drvPath4" = "$drvPath3" ]]
[[ -e "$drvPath3" ]]

# The modified derivation read back as JSON matches
bsd derivation show "$drvPath3" | jq .[] > "$TEST_HOME"/foo-read.json
diff "$TEST_HOME"/foo.json "$TEST_HOME"/foo-read.json
