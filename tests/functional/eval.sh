#!/usr/bin/env bash

source common.sh

clearStoreIfPossible

testStdinHeredoc=$(bsd eval -f - <<EOF
{
  bar = 3 + 1;
  foo = 2 + 2;
}
EOF
)
[[ $testStdinHeredoc == '{ bar = 4; foo = 4; }' ]]

bsd eval --expr 'assert 1 + 2 == 3; true'

[[ $(bsd eval int -f "./eval.nix") == 123 ]]
[[ $(bsd eval str -f "./eval.nix") == '"foo\nbar"' ]]
[[ $(bsd eval str --raw -f "./eval.nix") == $'foo\nbar' ]]
[[ "$(bsd eval attr -f "./eval.nix")" == '{ foo = "bar"; }' ]]
[[ $(bsd eval attr --json -f "./eval.nix") == '{"foo":"bar"}' ]]
[[ $(bsd eval int -f - < "./eval.nix") == 123 ]]
[[ "$(bsd eval --expr '{"assert"=1;bar=2;}')" == '{ "assert" = 1; bar = 2; }' ]]

# Check if toFile can be utilized during restricted eval
[[ $(bsd eval --restrict-eval --expr 'import (builtins.toFile "source" "42")') == 42 ]]

bsd-instantiate --eval -E 'assert 1 + 2 == 3; true'
[[ $(bsd-instantiate -A int --eval "./eval.nix") == 123 ]]
[[ $(bsd-instantiate -A str --eval "./eval.nix") == '"foo\nbar"' ]]
[[ $(bsd-instantiate -A str --raw --eval "./eval.nix") == $'foo\nbar' ]]
[[ "$(bsd-instantiate -A attr --eval "./eval.nix")" == '{ foo = "bar"; }' ]]
[[ $(bsd-instantiate -A attr --eval --json "./eval.nix") == '{"foo":"bar"}' ]]
[[ $(bsd-instantiate -A int --eval - < "./eval.nix") == 123 ]]
[[ "$(bsd-instantiate --eval -E '{"assert"=1;bar=2;}')" == '{ "assert" = 1; bar = 2; }' ]]

# Check that symlink cycles don't cause a hang.
ln -sfn cycle.nix "$TEST_ROOT/cycle.nix"
(! bsd eval --file "$TEST_ROOT/cycle.nix")

# --file and --pure-eval don't mix.
expectStderr 1 bsd eval --pure-eval --file "$TEST_ROOT/cycle.nix" | grepQuiet "not compatible"

# Check that relative symlinks are resolved correctly.
mkdir -p "$TEST_ROOT/xyzzy" "$TEST_ROOT/foo"
ln -sfn ../xyzzy "$TEST_ROOT/foo/bar"
printf 123 > "$TEST_ROOT/xyzzy/default.nix"
[[ $(bsd eval --impure --expr "import $TEST_ROOT/foo/bar") = 123 ]]

# Test --arg-from-file.
[[ "$(bsd eval --raw --arg-from-file foo "${config_bsd}" --expr '{ foo }: { inherit foo; }' foo)" = "$(cat "${config_bsd}")" ]]

# Check that special(-ish) files are drained.
if [[ -e /proc/version ]]; then
    [[ "$(bsd eval --raw --arg-from-file foo /proc/version --expr '{ foo }: { inherit foo; }' foo)" = "$(cat /proc/version)" ]]
fi

# Test --arg-from-stdin.
[[ "$(echo bla | bsd eval --raw --arg-from-stdin foo --expr '{ foo }: { inherit foo; }' foo)" = bla ]]

# Test that unknown settings are warned about
out="$(expectStderr 0 bsd eval --option foobar baz --expr '""' --raw)"
[[ "$(echo "$out" | grep -c foobar)" = 1 ]]

# Test flag alias
out="$(bsd eval --expr '{}' --build-cores 1)"
[[ "$(echo "$out" | wc -l)" = 1 ]]
