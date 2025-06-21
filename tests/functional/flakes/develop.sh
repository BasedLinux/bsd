#!/usr/bin/env bash

source ../common.sh

TODO_BasedLinux

clearStore
rm -rf "$TEST_HOME/.cache" "$TEST_HOME/.config" "$TEST_HOME/.local"

# Create flake under test.
cp ../shell-hello.bsd "$config_bsd" "$TEST_HOME/"
cat <<EOF >"$TEST_HOME/flake.bsd"
{
    inputs.bsdpkgs.url = "$TEST_HOME/bsdpkgs";
    outputs = {self, bsdpkgs}: {
      packages.$system.hello = (import ./config.bsd).mkDerivation {
        name = "hello";
        outputs = [ "out" "dev" ];
        meta.outputsToInstall = [ "out" ];
        buildCommand = "";
      };
    };
}
EOF

# Create fake bsdpkgs flake.
mkdir -p "$TEST_HOME/bsdpkgs"
cp "${config_bsd}" ../shell.bsd "$TEST_HOME/bsdpkgs"

cat <<EOF >"$TEST_HOME/bsdpkgs/flake.bsd"
{
    outputs = {self}: {
      legacyPackages.$system.bashInteractive = (import ./shell.bsd {}).bashInteractive;
    };
}
EOF

cd "$TEST_HOME"

# Test whether `bsd develop` passes through environment variables.
[[ "$(
    ENVVAR=a bsd develop --no-write-lock-file .#hello <<EOF
echo "\$ENVVAR"
EOF
)" = "a" ]]

# Test whether `bsd develop --ignore-env` does _not_ pass through environment variables.
[[ -z "$(
    ENVVAR=a bsd develop --ignore-env --no-write-lock-file .#hello <<EOF
echo "\$ENVVAR"
EOF
)" ]]

# Test wether `--keep-env-var` keeps the environment variable.
(
  expect='BAR'
  got="$(FOO='BAR' bsd develop --ignore-env --keep-env-var FOO --no-write-lock-file .#hello <<EOF
echo "\$FOO"
EOF
)"
  [[ "$got" == "$expect" ]]
)

# Test wether duplicate `--keep-env-var` keeps the environment variable.
(
  expect='BAR'
  got="$(FOO='BAR' bsd develop --ignore-env --keep-env-var FOO --keep-env-var FOO --no-write-lock-file .#hello <<EOF
echo "\$FOO"
EOF
)"
  [[ "$got" == "$expect" ]]
)

# Test wether `--set-env-var` sets the environment variable.
(
  expect='BAR'
  got="$(bsd develop --ignore-env --set-env-var FOO 'BAR' --no-write-lock-file .#hello <<EOF
echo "\$FOO"
EOF
)"
  [[ "$got" == "$expect" ]]
)

# Test that `--set-env-var` overwrites previously set variables.
(
  expect='BLA'
  got="$(FOO='BAR' bsd develop --set-env-var FOO 'BLA' --no-write-lock-file .#hello <<EOF
echo "\$FOO"
EOF
)"
  [[ "$got" == "$expect" ]]
)

# Test that multiple `--set-env-var` work.
(
  expect='BARFOO'
  got="$(bsd develop --set-env-var FOO 'BAR' --set-env-var BAR 'FOO' --no-write-lock-file .#hello <<EOF | tr -d '\n'
echo "\$FOO"
echo "\$BAR"
EOF
)"
  [[ "$got" == "$expect" ]]
)

# Check that we throw an error when `--keep-env-var` is used without `--ignore-env`.
expectStderr 1 bsd develop --keep-env-var FOO .#hello |
  grepQuiet "error: --keep-env-var does not make sense without --ignore-env"

# Check that we throw an error when `--unset-env-var` is used with `--ignore-env`.
expectStderr 1 bsd develop --ignore-env --unset-env-var FOO .#hello |
  grepQuiet "error: --unset-env-var does not make sense with --ignore-env"

# Test wether multiple occurances of `--set-env-var` throws.
expectStderr 1 bsd develop --set-env-var FOO 'BAR' --set-env-var FOO 'BLA' --no-write-lock-file .#hello |
  grepQuiet "error: Duplicate definition of environment variable 'FOO' with '--set-env-var' is ambiguous"

# Test wether similar `--unset-env-var` and `--set-env-var` throws.
expectStderr 1 bsd develop --set-env-var FOO 'BAR' --unset-env-var FOO --no-write-lock-file .#hello |
  grepQuiet "error: Cannot unset environment variable 'FOO' that is set with '--set-env-var'"

expectStderr 1 bsd develop --unset-env-var FOO --set-env-var FOO 'BAR' --no-write-lock-file .#hello |
  grepQuiet "error: Cannot set environment variable 'FOO' that is unset with '--unset-env-var'"

# Check that multiple `--ignore-env`'s are okay.
expectStderr 0 bsd develop --ignore-env --set-env-var FOO 'BAR' --ignore-env .#hello < /dev/null

# Determine the bashInteractive executable.
bsd build --no-write-lock-file './bsdpkgs#bashInteractive' --out-link ./bash-interactive
BASH_INTERACTIVE_EXECUTABLE="$PWD/bash-interactive/bin/bash"

# Test whether `bsd develop` sets `SHELL` to bsdpkgs#bashInteractive shell.
[[ "$(
    SHELL=custom bsd develop --no-write-lock-file .#hello <<EOF
echo "\$SHELL"
EOF
)" -ef "$BASH_INTERACTIVE_EXECUTABLE" ]]

# Test whether `bsd develop` with ignore environment sets `SHELL` to bsdpkgs#bashInteractive shell.
[[ "$(
    SHELL=custom bsd develop --ignore-env --no-write-lock-file .#hello <<EOF
echo "\$SHELL"
EOF
)" -ef "$BASH_INTERACTIVE_EXECUTABLE" ]]

clearStore
