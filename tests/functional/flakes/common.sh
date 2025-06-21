# shellcheck shell=bash

source ../common.sh

# shellcheck disable=SC2034 # this variable is used by tests that source this file
registry=$TEST_ROOT/registry.json

writeSimpleFlake() {
    local flakeDir="$1"
    cat > "$flakeDir/flake.bsd" <<EOF
{
  description = "Bla bla";

  outputs = inputs: rec {
    packages.$system = rec {
      foo = import ./simple.bsd;
      fooScript = (import ./shell.bsd {}).foo;
      default = foo;
    };
    packages.someOtherSystem = rec {
      foo = import ./simple.bsd;
      default = foo;
    };

    # To test "bsd flake init".
    legacyPackages.$system.hello = import ./simple.bsd;

    parent = builtins.dirOf ./.;

    baseName = builtins.baseNameOf ./.;

    root = ./.;
  };
}
EOF

    cp ../simple.bsd ../shell.bsd ../simple.builder.sh "${config_bsd}" "$flakeDir/"
}

createSimpleGitFlake() {
    requireGit
    local flakeDir="$1"
    writeSimpleFlake "$flakeDir"
    git -C "$flakeDir" add flake.bsd simple.bsd shell.bsd simple.builder.sh config.bsd
    git -C "$flakeDir" commit -m 'Initial'
}

# Create a simple Git flake and add it to the registry as "flake1".
createFlake1() {
    flake1Dir="$TEST_ROOT/flake1"
    createGitRepo "$flake1Dir" ""
    createSimpleGitFlake "$flake1Dir"
    bsd registry add --registry "$registry" flake1 "git+file://$flake1Dir"
}

createFlake2() {
    flake2Dir="$TEST_ROOT/flake 2"
    percentEncodedFlake2Dir="$TEST_ROOT/flake%202"

    # Give one repo a non-main initial branch.
    createGitRepo "$flake2Dir" "--initial-branch=main"

    cat > "$flake2Dir/flake.bsd" <<EOF
{
  description = "Fnord";

  outputs = { self, flake1 }: rec {
    packages.$system.bar = flake1.packages.$system.foo;
  };
}
EOF

    git -C "$flake2Dir" add flake.bsd
    git -C "$flake2Dir" commit -m 'Initial'

    bsd registry add --registry "$registry" flake2 "git+file://$percentEncodedFlake2Dir"
}

writeDependentFlake() {
    local flakeDir="$1"
    cat > "$flakeDir/flake.bsd" <<EOF
{
  outputs = { self, flake1 }: {
    packages.$system.default = flake1.packages.$system.default;
    expr = assert builtins.pathExists ./flake.lock; 123;
  };
}
EOF
}

writeIfdFlake() {
    local flakeDir="$1"
    cat > "$flakeDir/flake.bsd" <<EOF
{
  outputs = { self }: {
    packages.$system.default = import ./ifd.bsd;
  };
}
EOF

    cp -n ../ifd.bsd ../dependencies.bsd ../dependencies.builder0.sh "${config_bsd}" "$flakeDir/"
}

writeTrivialFlake() {
    local flakeDir="$1"
    cat > "$flakeDir/flake.bsd" <<EOF
{
  outputs = { self }: {
    expr = 123;
  };
}
EOF
}

initGitRepo() {
    local repo="$1"
    local extraArgs="${2-}"

    # shellcheck disable=SC2086 # word splitting of extraArgs is intended
    git -C "$repo" init $extraArgs
    git -C "$repo" config user.email "foobar@example.com"
    git -C "$repo" config user.name "Foobar"
}

createGitRepo() {
    local repo="$1"
    local extraArgs="${2-}"

    rm -rf "$repo" "$repo".tmp
    mkdir -p "$repo"

    # shellcheck disable=SC2086 # word splitting of extraArgs is intended
    initGitRepo "$repo" $extraArgs
}
