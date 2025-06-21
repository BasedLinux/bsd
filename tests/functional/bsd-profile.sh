#!/usr/bin/env bash

source common.sh

TODO_BasedLinux

clearStore
clearProfiles

enableFeatures "ca-derivations"
restartDaemon

# Make a flake.
flake1Dir=$TEST_ROOT/flake1
mkdir -p $flake1Dir

cat > $flake1Dir/flake.nix <<EOF
{
  description = "Bla bla";

  outputs = { self }: with import ./config.nix; rec {
    packages.$system.default = mkDerivation {
      name = "profile-test-\${builtins.readFile ./version}";
      outputs = [ "out" "man" "dev" ];
      builder = builtins.toFile "builder.sh"
        ''
          mkdir -p \$out/bin
          cat > \$out/bin/hello <<EOF
          #! ${shell}
          echo Hello \${builtins.readFile ./who}
          EOF
          chmod +x \$out/bin/hello
          echo DONE
          mkdir -p \$man/share/man
          mkdir -p \$dev/include
        '';
      __contentAddressed = import ./ca.nix;
      outputHashMode = "recursive";
      outputHashAlgo = "sha256";
      meta.outputsToInstall = [ "out" "man" ];
    };
  };
}
EOF

printf World > $flake1Dir/who
printf 1.0 > $flake1Dir/version
printf false > $flake1Dir/ca.nix

cp "${config_bsd}" $flake1Dir/

# Test upgrading from bsd-env.
bsd-env -f ./user-envs.nix -i foo-1.0
bsd profile list | grep -A2 'Name:.*foo' | grep 'Store paths:.*foo-1.0'
bsd profile add $flake1Dir -L
bsd profile list | grep -A4 'Name:.*flake1' | grep 'Locked flake URL:.*narHash'
[[ $($TEST_HOME/.nix-profile/bin/hello) = "Hello World" ]]
[ -e $TEST_HOME/.nix-profile/share/man ]
(! [ -e $TEST_HOME/.nix-profile/include ])
bsd profile history
bsd profile history | grep "packages.$system.default: ∅ -> 1.0"
bsd profile diff-closures | grep 'env-manifest.nix: ε → ∅'

# Test XDG Base Directories support
export NIX_CONFIG="use-xdg-base-directories = true"
bsd profile remove flake1 2>&1 | grep 'removed 1 packages'
bsd profile add $flake1Dir
[[ $($TEST_HOME/.local/state/bsd/profile/bin/hello) = "Hello World" ]]
unset NIX_CONFIG

# Test conflicting package add.
bsd profile add $flake1Dir 2>&1 | grep "warning: 'flake1' is already added"

# Test upgrading a package.
printf BasedLinux > $flake1Dir/who
printf 2.0 > $flake1Dir/version
bsd profile upgrade flake1
[[ $($TEST_HOME/.nix-profile/bin/hello) = "Hello BasedLinux" ]]
bsd profile history | grep "packages.$system.default: 1.0, 1.0-man -> 2.0, 2.0-man"

# Test upgrading package using regular expression.
printf 2.1 > $flake1Dir/version
bsd profile upgrade --regex '.*'
[[ $(readlink $TEST_HOME/.nix-profile/bin/hello) =~ .*-profile-test-2\.1/bin/hello ]]
bsd profile rollback

# Test upgrading all packages
printf 2.2 > $flake1Dir/version
bsd profile upgrade --all
[[ $(readlink $TEST_HOME/.nix-profile/bin/hello) =~ .*-profile-test-2\.2/bin/hello ]]
bsd profile rollback
printf 1.0 > $flake1Dir/version

# Test --all exclusivity.
assertStderr bsd --offline profile upgrade --all foo << EOF
error: --all cannot be used with package names or regular expressions.
Try 'bsd --help' for more information.
EOF

# Test matching no packages using literal package name.
assertStderr bsd --offline profile upgrade this_package_is_not_installed << EOF
warning: Package name 'this_package_is_not_installed' does not match any packages in the profile.
warning: No packages to upgrade. Use 'bsd profile list' to see the current profile.
EOF

# Test matching no packages using regular expression.
assertStderr bsd --offline profile upgrade --regex '.*unknown_package.*' << EOF
warning: Regex '.*unknown_package.*' does not match any packages in the profile.
warning: No packages to upgrade. Use 'bsd profile list' to see the current profile.
EOF

# Test removing all packages using regular expression.
bsd profile remove --regex '.*' 2>&1 | grep "removed 2 packages, kept 0 packages"
bsd profile rollback

# Test 'history', 'diff-closures'.
bsd profile diff-closures

# Test rollback.
printf World > $flake1Dir/who
bsd profile upgrade flake1
printf BasedLinux > $flake1Dir/who
bsd profile upgrade flake1
bsd profile rollback
[[ $($TEST_HOME/.nix-profile/bin/hello) = "Hello World" ]]

# Test uninstall.
[ -e $TEST_HOME/.nix-profile/bin/foo ]
bsd profile remove foo 2>&1 | grep 'removed 1 packages'
(! [ -e $TEST_HOME/.nix-profile/bin/foo ])
bsd profile history | grep 'foo: 1.0 -> ∅'
bsd profile diff-closures | grep 'Version 3 -> 4'

# Test installing a non-flake package.
bsd profile add --file ./simple.nix ''
[[ $(cat $TEST_HOME/.nix-profile/hello) = "Hello World!" ]]
bsd profile remove simple 2>&1 | grep 'removed 1 packages'
bsd profile add $(bsd-build --no-out-link ./simple.nix)
[[ $(cat $TEST_HOME/.nix-profile/hello) = "Hello World!" ]]

# Test packages with same name from different sources
mkdir $TEST_ROOT/simple-too
cp ./simple.nix "${config_bsd}" simple.builder.sh $TEST_ROOT/simple-too
bsd profile add --file $TEST_ROOT/simple-too/simple.nix ''
bsd profile list | grep -A4 'Name:.*simple' | grep 'Name:.*simple-1'
bsd profile remove simple 2>&1 | grep 'removed 1 packages'
bsd profile remove simple-1 2>&1 | grep 'removed 1 packages'

# Test wipe-history.
bsd profile wipe-history
[[ $(bsd profile history | grep Version | wc -l) -eq 1 ]]

# Test upgrade to CA package.
printf true > $flake1Dir/ca.nix
printf 3.0 > $flake1Dir/version
bsd profile upgrade flake1
bsd profile history | grep "packages.$system.default: 1.0, 1.0-man -> 3.0, 3.0-man"

# Test new install of CA package.
bsd profile remove flake1 2>&1 | grep 'removed 1 packages'
printf 4.0 > $flake1Dir/version
printf Utrecht > $flake1Dir/who
bsd profile add $flake1Dir
[[ $($TEST_HOME/.nix-profile/bin/hello) = "Hello Utrecht" ]]
[[ $(bsd path-info --json $(realpath $TEST_HOME/.nix-profile/bin/hello) | jq -r .[].ca) =~ fixed:r:sha256: ]]

# Override the outputs.
bsd profile remove simple flake1
bsd profile add "$flake1Dir^*"
[[ $($TEST_HOME/.nix-profile/bin/hello) = "Hello Utrecht" ]]
[ -e $TEST_HOME/.nix-profile/share/man ]
[ -e $TEST_HOME/.nix-profile/include ]

printf Bsd > $flake1Dir/who
bsd profile list
bsd profile upgrade flake1
[[ $($TEST_HOME/.nix-profile/bin/hello) = "Hello Bsd" ]]
[ -e $TEST_HOME/.nix-profile/share/man ]
[ -e $TEST_HOME/.nix-profile/include ]

bsd profile remove flake1 2>&1 | grep 'removed 1 packages'
bsd profile add "$flake1Dir^man"
(! [ -e $TEST_HOME/.nix-profile/bin/hello ])
[ -e $TEST_HOME/.nix-profile/share/man ]
(! [ -e $TEST_HOME/.nix-profile/include ])

# test priority
bsd profile remove flake1 2>&1 | grep 'removed 1 packages'

# Make another flake.
flake2Dir=$TEST_ROOT/flake2
printf World > $flake1Dir/who
cp -r $flake1Dir $flake2Dir
printf World2 > $flake2Dir/who

bsd profile add $flake1Dir
[[ $($TEST_HOME/.nix-profile/bin/hello) = "Hello World" ]]
expect 1 bsd profile add $flake2Dir
diff -u <(
    bsd --offline profile install $flake2Dir 2>&1 1> /dev/null \
        | grep -vE "^warning: " \
        | grep -vE "^error \(ignored\): " \
        || true
) <(cat << EOF
error: An existing package already provides the following file:

         $(bsd build --no-link --print-out-paths ${flake1Dir}"#default.out")/bin/hello

       This is the conflicting file from the new package:

         $(bsd build --no-link --print-out-paths ${flake2Dir}"#default.out")/bin/hello

       To remove the existing package:

         bsd profile remove flake1

       The new package can also be added next to the existing one by assigning a different priority.
       The conflicting packages have a priority of 5.
       To prioritise the new package:

         bsd profile add path:${flake2Dir}#packages.${system}.default --priority 4

       To prioritise the existing package:

         bsd profile add path:${flake2Dir}#packages.${system}.default --priority 6
EOF
)
[[ $($TEST_HOME/.nix-profile/bin/hello) = "Hello World" ]]
bsd profile add $flake2Dir --priority 100
[[ $($TEST_HOME/.nix-profile/bin/hello) = "Hello World" ]]
bsd profile add $flake2Dir --priority 0
[[ $($TEST_HOME/.nix-profile/bin/hello) = "Hello World2" ]]
# bsd profile add $flake1Dir --priority 100
# [[ $($TEST_HOME/.nix-profile/bin/hello) = "Hello World" ]]

# Ensure that conflicts are handled properly even when the installables aren't
# flake references.
# Regression test for https://github.com/BasedLinux/bsd/issues/8284
clearProfiles
bsd profile add $(bsd build $flake1Dir --no-link --print-out-paths)
expect 1 bsd profile add --impure --expr "(builtins.getFlake ''$flake2Dir'').packages.$system.default"

# Test upgrading from profile version 2.
clearProfiles
mkdir -p $TEST_ROOT/import-profile
outPath=$(bsd build --no-link --print-out-paths $flake1Dir/flake.nix^out)
printf '{ "version": 2, "elements": [ { "active": true, "attrPath": "legacyPackages.x86_64-linux.hello", "originalUrl": "flake:bsdpkgs", "outputs": null, "priority": 5, "storePaths": [ "%s" ], "url": "github:BasedLinux/bsdpkgs/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" } ] }' "$outPath" > $TEST_ROOT/import-profile/manifest.json
bsd build --profile $TEST_HOME/.nix-profile $(bsd store add-path $TEST_ROOT/import-profile) --no-link
bsd profile list | grep -A4 'Name:.*hello' | grep "Store paths:.*$outPath"
bsd profile remove hello 2>&1 | grep 'removed 1 packages, kept 0 packages'
