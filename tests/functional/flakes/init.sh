#!/usr/bin/env bash

source ./common.sh

requireGit

templatesDir=$TEST_ROOT/templates
flakeDir=$TEST_ROOT/flake
bsdpkgsDir=$TEST_ROOT/bsdpkgs

bsd registry add --registry "$registry" templates "git+file://$templatesDir"
bsd registry add --registry "$registry" bsdpkgs "git+file://$bsdpkgsDir"

createGitRepo "$bsdpkgsDir"
createSimpleGitFlake "$bsdpkgsDir"

# Test 'bsd flake init'.
createGitRepo "$templatesDir"

cat > "$templatesDir"/flake.nix <<EOF
{
  description = "Some templates";

  outputs = { self }: {
    templates = rec {
      trivial = {
        path = ./trivial;
        description = "A trivial flake";
        welcomeText = ''
            Welcome to my trivial flake
        '';
      };
      default = trivial;
    };
  };
}
EOF

mkdir "$templatesDir/trivial"

cat > "$templatesDir"/trivial/flake.nix <<EOF
{
  description = "A flake for building Hello World";

  outputs = { self, bsdpkgs }: {
    packages.$system = rec {
      hello = bsdpkgs.legacyPackages.$system.hello;
      default = hello;
    };
  };
}
EOF
echo a > "$templatesDir/trivial/a"
echo b > "$templatesDir/trivial/b"

git -C "$templatesDir" add flake.nix trivial/
git -C "$templatesDir" commit -m 'Initial'

bsd flake check templates
bsd flake show templates
bsd flake show templates --json | jq

createGitRepo "$flakeDir"
(cd "$flakeDir" && bsd flake init)
(cd "$flakeDir" && bsd flake init) # check idempotence
git -C "$flakeDir" add flake.nix
bsd flake check "$flakeDir"
bsd flake show "$flakeDir"
bsd flake show "$flakeDir" --json | jq
git -C "$flakeDir" commit -a -m 'Initial'

# Test 'bsd flake init' with benign conflicts
createGitRepo "$flakeDir"
echo a > "$flakeDir/a"
(cd "$flakeDir" && bsd flake init) # check idempotence

# Test 'bsd flake init' with conflicts
createGitRepo "$flakeDir"
echo b > "$flakeDir/a"
pushd "$flakeDir"
(! bsd flake init) |& grep "refusing to overwrite existing file '$flakeDir/a'"
popd
git -C "$flakeDir" commit -a -m 'Changed'

# Test 'bsd flake new'.
rm -rf "$flakeDir"
bsd flake new -t templates#trivial "$flakeDir"
bsd flake new -t templates#trivial "$flakeDir" # check idempotence
bsd flake check "$flakeDir"
