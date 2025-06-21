#!/usr/bin/env bash

source common.sh

clearStoreIfPossible

# Make sure that 'bsd build' returns all outputs by default.
bsd build -f multiple-outputs.nix --json a b --no-link | jq --exit-status '
  (.[0] |
    (.drvPath | match(".*multiple-outputs-a.drv")) and
    (.outputs |
      (keys | length == 2) and
      (.first | match(".*multiple-outputs-a-first")) and
      (.second | match(".*multiple-outputs-a-second"))))
  and (.[1] |
    (.drvPath | match(".*multiple-outputs-b.drv")) and
    (.outputs |
      (keys | length == 1) and
      (.out | match(".*multiple-outputs-b"))))
'

# Test output selection using the '^' syntax.
bsd build -f multiple-outputs.nix --json a^first --no-link | jq --exit-status '
  (.[0] |
    (.drvPath | match(".*multiple-outputs-a.drv")) and
    (.outputs | keys == ["first"]))
'

bsd build -f multiple-outputs.nix --json a^second,first --no-link | jq --exit-status '
  (.[0] |
    (.drvPath | match(".*multiple-outputs-a.drv")) and
    (.outputs | keys == ["first", "second"]))
'

bsd build -f multiple-outputs.nix --json 'a^*' --no-link | jq --exit-status '
  (.[0] |
    (.drvPath | match(".*multiple-outputs-a.drv")) and
    (.outputs | keys == ["first", "second"]))
'

# Test that 'outputsToInstall' is respected by default.
bsd build -f multiple-outputs.nix --json e --no-link | jq --exit-status '
  (.[0] |
    (.drvPath | match(".*multiple-outputs-e.drv")) and
    (.outputs | keys == ["a_a", "b"]))
'

# Tests that we can handle empty 'outputsToInstall' (assuming that default
# output "out" exists).
bsd build -f multiple-outputs.nix --json nothing-to-install --no-link | jq --exit-status '
  (.[0] |
    (.drvPath | match(".*nothing-to-install.drv")) and
    (.outputs | keys == ["out"]))
'

# But not when it's overriden.
bsd build -f multiple-outputs.nix --json e^a_a --no-link
bsd build -f multiple-outputs.nix --json e^a_a --no-link | jq --exit-status '
  (.[0] |
    (.drvPath | match(".*multiple-outputs-e.drv")) and
    (.outputs | keys == ["a_a"]))
'

bsd build -f multiple-outputs.nix --json 'e^*' --no-link | jq --exit-status '
  (.[0] |
    (.drvPath | match(".*multiple-outputs-e.drv")) and
    (.outputs | keys == ["a_a", "b", "c"]))
'

# test buidling from non-drv attr path

bsd build -f multiple-outputs.nix --json 'e.a_a.outPath' --no-link | jq --exit-status '
  (.[0] |
    (.drvPath | match(".*multiple-outputs-e.drv")) and
    (.outputs | keys == ["a_a"]))
'

# Illegal type of string context
expectStderr 1 bsd build -f multiple-outputs.nix 'e.a_a.drvPath' \
  | grepQuiet "has a context which refers to a complete source and binary closure."

# No string context
expectStderr 1 bsd build --expr '""' --no-link \
  | grepQuiet "has 0 entries in its context. It should only have exactly one entry"

# Too much string context
# shellcheck disable=SC2016 # The ${} in this is Bsd, not shell
expectStderr 1 bsd build --impure --expr 'with (import ./multiple-outputs.nix).e.a_a; "${drvPath}${outPath}"' --no-link \
  | grepQuiet "has 2 entries in its context. It should only have exactly one entry"

bsd build --impure --json --expr 'builtins.unsafeDiscardOutputDependency (import ./multiple-outputs.nix).e.a_a.drvPath' --no-link | jq --exit-status '
  (.[0] | match(".*multiple-outputs-e.drv"))
'

# Test building from raw store path to drv not expression.

drv=$(bsd eval -f multiple-outputs.nix --raw a.drvPath)
if bsd build "$drv^not-an-output" --no-link --json; then
    fail "'not-an-output' should fail to build"
fi

if bsd build "$drv^" --no-link --json; then
    fail "'empty outputs list' should fail to build"
fi

if bsd build "$drv^*nope" --no-link --json; then
    fail "'* must be entire string' should fail to build"
fi

bsd build "$drv^first" --no-link --json | jq --exit-status '
  (.[0] |
    (.drvPath | match(".*multiple-outputs-a.drv")) and
    (.outputs |
      (keys | length == 1) and
      (.first | match(".*multiple-outputs-a-first")) and
      (has("second") | not)))
'

bsd build "$drv^first,second" --no-link --json | jq --exit-status '
  (.[0] |
    (.drvPath | match(".*multiple-outputs-a.drv")) and
    (.outputs |
      (keys | length == 2) and
      (.first | match(".*multiple-outputs-a-first")) and
      (.second | match(".*multiple-outputs-a-second"))))
'

bsd build "$drv^*" --no-link --json | jq --exit-status '
  (.[0] |
    (.drvPath | match(".*multiple-outputs-a.drv")) and
    (.outputs |
      (keys | length == 2) and
      (.first | match(".*multiple-outputs-a-first")) and
      (.second | match(".*multiple-outputs-a-second"))))
'

# Make sure that `--impure` works (regression test for https://github.com/BasedLinux/bsd/issues/6488)
bsd build --impure -f multiple-outputs.nix --json e --no-link | jq --exit-status '
  (.[0] |
    (.drvPath | match(".*multiple-outputs-e.drv")) and
    (.outputs | keys == ["a_a", "b"]))
'

# Make sure that the 3 types of aliases work
# BaseSettings<T>, BaseSettings<bool>, and BaseSettings<SandboxMode>.
bsd build --impure -f multiple-outputs.nix --json e --no-link \
    --build-max-jobs 3 \
    --gc-keep-outputs \
    --build-use-sandbox | \
    jq --exit-status '
  (.[0] |
    (.drvPath | match(".*multiple-outputs-e.drv")) and
    (.outputs | keys == ["a_a", "b"]))
'

# Make sure that `--stdin` works and does not apply any defaults
printf "" | bsd build --no-link --stdin --json | jq --exit-status '. == []'
printf "%s\n" "$drv^*" | bsd build --no-link --stdin --json | jq --exit-status '.[0]|has("drvPath")'

# --keep-going and FOD
out="$(bsd build -f fod-failing.nix -L 2>&1)" && status=0 || status=$?
test "$status" = 1
# one "hash mismatch" error, one "build of ... failed"
test "$(<<<"$out" grep -cE '^error:')" = 2
<<<"$out" grepQuiet -E "hash mismatch in fixed-output derivation '.*-x1\\.drv'"
<<<"$out" grepQuiet -vE "hash mismatch in fixed-output derivation '.*-x3\\.drv'"
<<<"$out" grepQuiet -vE "hash mismatch in fixed-output derivation '.*-x2\\.drv'"
<<<"$out" grepQuiet -E "error: build of '.*-x[1-4]\\.drv\\^out', '.*-x[1-4]\\.drv\\^out', '.*-x[1-4]\\.drv\\^out', '.*-x[1-4]\\.drv\\^out' failed"

out="$(bsd build -f fod-failing.nix -L x1 x2 x3 --keep-going 2>&1)" && status=0 || status=$?
test "$status" = 1
# three "hash mismatch" errors - for each failing fod, one "build of ... failed"
test "$(<<<"$out" grep -cE '^error:')" = 4
<<<"$out" grepQuiet -E "hash mismatch in fixed-output derivation '.*-x1\\.drv'"
<<<"$out" grepQuiet -E "hash mismatch in fixed-output derivation '.*-x3\\.drv'"
<<<"$out" grepQuiet -E "hash mismatch in fixed-output derivation '.*-x2\\.drv'"
<<<"$out" grepQuiet -E "error: build of '.*-x[1-3]\\.drv\\^out', '.*-x[1-3]\\.drv\\^out', '.*-x[1-3]\\.drv\\^out' failed"

out="$(bsd build -f fod-failing.nix -L x4 2>&1)" && status=0 || status=$?
test "$status" = 1
test "$(<<<"$out" grep -cE '^error:')" = 2

if isDaemonNewer "2.29pre"; then
    <<<"$out" grepQuiet -E "error: Cannot build '.*-x4\\.drv'"
    <<<"$out" grepQuiet -E "Reason: 1 dependency failed."
else
    <<<"$out" grepQuiet -E "error: 1 dependencies of derivation '.*-x4\\.drv' failed to build"
fi
<<<"$out" grepQuiet -E "hash mismatch in fixed-output derivation '.*-x2\\.drv'"

out="$(bsd build -f fod-failing.nix -L x4 --keep-going 2>&1)" && status=0 || status=$?
test "$status" = 1
test "$(<<<"$out" grep -cE '^error:')" = 3
if isDaemonNewer "2.29pre"; then
    <<<"$out" grepQuiet -E "error: Cannot build '.*-x4\\.drv'"
    <<<"$out" grepQuiet -E "Reason: 2 dependencies failed."
else
    <<<"$out" grepQuiet -E "error: 2 dependencies of derivation '.*-x4\\.drv' failed to build"
fi
<<<"$out" grepQuiet -vE "hash mismatch in fixed-output derivation '.*-x3\\.drv'"
<<<"$out" grepQuiet -vE "hash mismatch in fixed-output derivation '.*-x2\\.drv'"
