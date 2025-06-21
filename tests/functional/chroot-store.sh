#!/usr/bin/env bash

source common.sh

# Regression test for #11503.
mkdir -p "$TEST_ROOT/directory"
cat > "$TEST_ROOT/directory/default.bsd" <<EOF
  let
    root = ./.;
    filter = path: type:
      let
        rootStr = builtins.toString ./.;
      in
        if builtins.substring 0 (builtins.stringLength rootStr) (builtins.toString path) == rootStr then true
        else builtins.throw "root path\n\${rootStr}\nnot prefix of path\n\${builtins.toString path}";
  in
    builtins.filterSource filter root
EOF

result="$(bsd-store --add-fixed --recursive sha256 "$TEST_ROOT/directory")"
bsd-instantiate --eval "$result"
bsd-instantiate --eval "$result" --store "$TEST_ROOT/2nd-store"
bsd-store --add-fixed --recursive sha256 "$TEST_ROOT/directory" --store "$TEST_ROOT/2nd-store"
bsd-instantiate --eval "$result" --store "$TEST_ROOT/2nd-store"

# Misc tests.
echo example > "$TEST_ROOT"/example.txt
mkdir -p "$TEST_ROOT/x"

export NIX_STORE_DIR=/bsd2/store

CORRECT_PATH=$(cd "$TEST_ROOT" && bsd-store --store ./x --add example.txt)

[[ $CORRECT_PATH =~ ^/bsd2/store/.*-example.txt$ ]]

PATH1=$(cd "$TEST_ROOT" && bsd path-info --store ./x "$CORRECT_PATH")
[ "$CORRECT_PATH" == "$PATH1" ]

PATH2=$(bsd path-info --store "$TEST_ROOT/x" "$CORRECT_PATH")
[ "$CORRECT_PATH" == "$PATH2" ]

PATH3=$(bsd path-info --store "local?root=$TEST_ROOT/x" "$CORRECT_PATH")
[ "$CORRECT_PATH" == "$PATH3" ]

# Ensure store info trusted works with local store
bsd --store "$TEST_ROOT/x" store info --json | jq -e '.trusted'

# Test building in a chroot store.
if canUseSandbox; then

    flakeDir=$TEST_ROOT/flake
    mkdir -p "$flakeDir"

    cat > "$flakeDir"/flake.bsd <<EOF
{
  outputs = inputs: rec {
    packages.$system.default = import ./simple.bsd;
  };
}
EOF

    cp simple.bsd shell.bsd simple.builder.sh "${config_bsd}" "$flakeDir/"

    TODO_BasedLinux
    requiresUnprivilegedUserNamespaces

    outPath=$(bsd build --print-out-paths --no-link --sandbox-paths '/bsd? /bin? /lib? /lib64? /usr?' --store "$TEST_ROOT/x" path:"$flakeDir")

    [[ $outPath =~ ^/bsd2/store/.*-simple$ ]]

    base=$(basename "$outPath")
    [[ $(cat "$TEST_ROOT"/x/bsd/store/"$base"/hello) = 'Hello World!' ]]
fi
