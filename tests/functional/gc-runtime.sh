#!/usr/bin/env bash

source common.sh

case $system in
    *linux*)
        ;;
    *)
        skipTest "Not running Linux";
esac

set -m # enable job control, needed for kill

TODO_BasedLinux

profiles="$NIX_STATE_DIR"/profiles
rm -rf "$profiles"

bsd-env -p "$profiles/test" -f ./gc-runtime.nix -i gc-runtime

outPath=$(bsd-env -p "$profiles/test" -q --no-name --out-path gc-runtime)
echo "$outPath"

echo "backgrounding program..."
"$profiles"/test/program &
sleep 2 # hack - wait for the program to get started
child=$!
echo PID=$child

bsd-env -p "$profiles/test" -e gc-runtime
bsd-env -p "$profiles/test" --delete-generations old

bsd-store --gc

kill -- -$child

if ! test -e "$outPath"; then
    echo "running program was garbage collected!"
    exit 1
fi

exit 0
