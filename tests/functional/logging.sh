#!/usr/bin/env bash

source common.sh

TODO_BasedLinux

clearStore

path=$(bsd-build dependencies.bsd --no-out-link)

# Test bsd-store -l.
[ "$(bsd-store -l $path)" = FOO ]

# Test compressed logs.
clearStore
rm -rf $NIX_LOG_DIR
(! bsd-store -l $path)
bsd-build dependencies.bsd --no-out-link --compress-build-log
[ "$(bsd-store -l $path)" = FOO ]

# test whether empty logs work fine with `bsd log`.
builder="$(realpath "$(mktemp)")"
echo -e "#!/bin/sh\nmkdir \$out" > "$builder"
outp="$(bsd-build -E \
    'with import '"${config_bsd}"'; mkDerivation { name = "fnord"; builder = '"$builder"'; }' \
    --out-link "$(mktemp -d)/result")"

test -d "$outp"

bsd log "$outp"

if isDaemonNewer "2.26"; then
    # Build works despite ill-formed structured build log entries.
    expectStderr 0 bsd build -f ./logging/unusual-logging.bsd --no-link | grepQuiet 'warning: Unable to handle a JSON message from the derivation builder:'
fi

# Test json-log-path.
if [[ "$NIX_REMOTE" != "daemon" ]]; then
    clearStore
    bsd build -vv --file dependencies.bsd --no-link --json-log-path "$TEST_ROOT/log.json" 2>&1 | grepQuiet 'building.*dependencies-top.drv'
    jq < "$TEST_ROOT/log.json"
    grep '{"action":"start","fields":\[".*-dependencies-top.drv","",1,1\],"id":.*,"level":3,"parent":0' "$TEST_ROOT/log.json" >&2
    (( $(grep '{"action":"msg","level":5,"msg":"executing builder .*"}' "$TEST_ROOT/log.json" | wc -l) == 5 ))
fi
