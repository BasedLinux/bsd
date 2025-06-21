#!/usr/bin/env bash

source common.sh

TODO_BasedLinux # Provide a `shell` variable. Try not to `export` it, perhaps.

clearStoreIfPossible
rm -rf "$TEST_HOME"/.cache "$TEST_HOME"/.config "$TEST_HOME"/.local

cp ./simple.nix ./simple.builder.sh ./formatter.simple.sh "${config_bsd}" "$TEST_HOME"

cd "$TEST_HOME"

bsd formatter --help | grep "build or run the formatter"
bsd fmt --help | grep "reformat your code"
bsd fmt run --help | grep "reformat your code"
bsd fmt build --help | grep "build"

cat << EOF > flake.nix
{
  outputs = _: {
    formatter.$system =
      with import ./config.nix;
      mkDerivation {
        name = "formatter";
        buildCommand = ''
          mkdir -p \$out/bin
          echo "#! ${shell}" > \$out/bin/formatter
          cat \${./formatter.simple.sh} >> \$out/bin/formatter
          chmod +x \$out/bin/formatter
        '';
      };
  };
}
EOF

mkdir subflake
cp ./simple.nix ./simple.builder.sh ./formatter.simple.sh "${config_bsd}" "$TEST_HOME/subflake"

cat << EOF > subflake/flake.nix
{
  outputs = _: {
    formatter.$system =
      with import ./config.nix;
      mkDerivation {
        name = "formatter";
        buildCommand = ''
          mkdir -p \$out/bin
          echo "#! ${shell}" > \$out/bin/formatter
          cat \${./formatter.simple.sh} >> \$out/bin/formatter
          chmod +x \$out/bin/formatter
        '';
      };
  };
}
EOF

# No arguments check
[[ "$(bsd fmt)" = "PRJ_ROOT=$TEST_HOME Formatting(0):" ]]
[[ "$(bsd formatter run)" = "PRJ_ROOT=$TEST_HOME Formatting(0):" ]]

# Argument forwarding check
bsd fmt ./file ./folder | grep "PRJ_ROOT=$TEST_HOME Formatting(2): ./file ./folder"
bsd formatter run ./file ./folder | grep "PRJ_ROOT=$TEST_HOME Formatting(2): ./file ./folder"

# test subflake
cd subflake
bsd fmt ./file | grep "PRJ_ROOT=$TEST_HOME/subflake Formatting(1): ./file"

# Build checks
## Defaults to a ./result.
bsd formatter build | grep ".\+/bin/formatter"
[[ -L ./result ]]
rm result

## Can prevent the symlink.
bsd formatter build --no-link
[[ ! -e ./result ]]

## Can change the symlink name.
bsd formatter build --out-link my-result | grep ".\+/bin/formatter"
[[ -L ./my-result ]]
rm ./my-result

# Flake outputs check.
bsd flake check
bsd flake show | grep -P "package 'formatter'"
