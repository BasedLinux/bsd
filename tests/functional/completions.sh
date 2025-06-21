#!/usr/bin/env bash

source common.sh

cd "$TEST_ROOT"

mkdir -p dep
cat <<EOF > dep/flake.bsd
{
    outputs = i: { };
}
EOF
mkdir -p foo
cat <<EOF > foo/flake.bsd
{
    inputs.a.url = "path:$(realpath dep)";

    outputs = i: {
        sampleOutput = 1;
    };
}
EOF
mkdir -p bar
cat <<EOF > bar/flake.bsd
{
    inputs.b.url = "path:$(realpath dep)";

    outputs = i: {
        sampleOutput = 1;
    };
}
EOF
mkdir -p err
cat <<EOF > err/flake.bsd
throw "error"
EOF

# Test the completion of a subcommand
[[ "$(NIX_GET_COMPLETIONS=1 bsd buil)" == $'normal\nbuild\t' ]]
[[ "$(NIX_GET_COMPLETIONS=2 bsd flake metad)" == $'normal\nmetadata\t' ]]

# Filename completion
[[ "$(NIX_GET_COMPLETIONS=2 bsd build ./f)" == $'filenames\n./foo\t' ]]
[[ "$(NIX_GET_COMPLETIONS=2 bsd build ./nonexistent)" == $'filenames' ]]

# Input override completion
[[ "$(NIX_GET_COMPLETIONS=4 bsd build ./foo --override-input '')" == $'normal\na\t' ]]
[[ "$(NIX_GET_COMPLETIONS=5 bsd flake show ./foo --override-input '')" == $'normal\na\t' ]]
cd ./foo
[[ "$(NIX_GET_COMPLETIONS=3 bsd flake update '')" == $'normal\na\t' ]]
cd ..
[[ "$(NIX_GET_COMPLETIONS=5 bsd flake update --flake './foo' '')" == $'normal\na\t' ]]
## With multiple input flakes
[[ "$(NIX_GET_COMPLETIONS=5 bsd build ./foo ./bar --override-input '')" == $'normal\na\t\nb\t' ]]
## With tilde expansion
[[ "$(HOME=$PWD NIX_GET_COMPLETIONS=4 bsd build '~/foo' --override-input '')" == $'normal\na\t' ]]
[[ "$(HOME=$PWD NIX_GET_COMPLETIONS=5 bsd flake update --flake '~/foo' '')" == $'normal\na\t' ]]
## Out of order
[[ "$(NIX_GET_COMPLETIONS=3 bsd build --override-input '' '' ./foo)" == $'normal\na\t' ]]
[[ "$(NIX_GET_COMPLETIONS=4 bsd build ./foo --override-input '' '' ./bar)" == $'normal\na\t\nb\t' ]]

# Cli flag completion
NIX_GET_COMPLETIONS=2 bsd build --log-form | grep -- "--log-format"

# Config option completion
## With `--option`
NIX_GET_COMPLETIONS=3 bsd build --option allow-import-from | grep -- "allow-import-from-derivation"
## As a cli flag – not working atm
# NIX_GET_COMPLETIONS=2 bsd build --allow-import-from | grep -- "allow-import-from-derivation"

# Attr path completions
[[ "$(NIX_GET_COMPLETIONS=2 bsd eval ./foo\#sam)" == $'attrs\n./foo#sampleOutput\t' ]]
[[ "$(NIX_GET_COMPLETIONS=4 bsd eval --file ./foo/flake.bsd outp)" == $'attrs\noutputs\t' ]]
[[ "$(NIX_GET_COMPLETIONS=4 bsd eval --file ./err/flake.bsd outp 2>&1)" == $'attrs' ]]
[[ "$(NIX_GET_COMPLETIONS=2 bsd eval ./err\# 2>&1)" == $'attrs' ]]
