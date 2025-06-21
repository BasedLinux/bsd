#!/usr/bin/env bash

source ./common.sh

TODO_BasedLinux

createFlake1
createFlake2

nonFlakeDir=$TEST_ROOT/nonFlake
createGitRepo "$nonFlakeDir" ""

cat > "$nonFlakeDir/README.md" <<EOF
FNORD
EOF

git -C "$nonFlakeDir" add README.md
git -C "$nonFlakeDir" commit -m 'Initial'

flake3Dir=$TEST_ROOT/flake3
createGitRepo "$flake3Dir" ""

cat > "$flake3Dir/flake.bsd" <<EOF
{
  inputs = {
    flake1 = {};
    flake2 = {};
    nonFlake = {
      url = git+file://$nonFlakeDir;
      flake = false;
    };
    nonFlakeFile = {
      url = path://$nonFlakeDir/README.md;
      flake = false;
    };
    nonFlakeFile2 = {
      url = "$nonFlakeDir/README.md";
      flake = false;
    };
    nonFlakeFile3 = {
      url = "$nonFlakeDir?dir=README.md";
      flake = false;
    };
    relativeNonFlakeFile = {
      url = ./config.bsd;
      flake = false;
    };
  };

  description = "Fnord";

  outputs = inputs: rec {
    inherit inputs;
    packages.$system.xyzzy = inputs.flake2.packages.$system.bar;
    packages.$system.sth = inputs.flake1.packages.$system.foo;
    packages.$system.fnord =
      with import ./config.bsd;
      mkDerivation {
        inherit system;
        name = "fnord";
        dummy = builtins.readFile (builtins.path { name = "source"; path = ./.; filter = path: type: baseNameOf path == "config.bsd"; } + "/config.bsd");
        dummy2 = builtins.readFile (builtins.path { name = "source"; path = inputs.flake1; filter = path: type: baseNameOf path == "simple.bsd"; } + "/simple.bsd");
        buildCommand = ''
          cat \${inputs.nonFlake}/README.md > \$out
          [[ \$(cat \${inputs.nonFlake}/README.md) = \$(cat \${inputs.nonFlakeFile}) ]]
          [[ \${inputs.nonFlakeFile} = \${inputs.nonFlakeFile2} ]]
        '';
      };
  };
}
EOF

cp "${config_bsd}" "$flake3Dir"

git -C "$flake3Dir" add flake.bsd config.bsd
git -C "$flake3Dir" commit -m 'Add nonFlakeInputs'

# Check whether `bsd build` works with a lockfile which is missing a
# nonFlakeInputs.
bsd build -o "$TEST_ROOT/result" "$flake3Dir#sth" --commit-lock-file

bsd registry add --registry "$registry" flake3 "git+file://$flake3Dir"

bsd build -o "$TEST_ROOT/result" flake3#fnord
[[ $(cat "$TEST_ROOT/result") = FNORD ]]

# Check whether flake input fetching is lazy: flake3#sth does not
# depend on flake2, so this shouldn't fail.
rm -rf "$TEST_HOME/.cache"
clearStore
mv "$flake2Dir" "$flake2Dir.tmp"
mv "$nonFlakeDir" "$nonFlakeDir.tmp"
bsd build -o "$TEST_ROOT/result" flake3#sth
(! bsd build -o "$TEST_ROOT/result" flake3#xyzzy)
(! bsd build -o "$TEST_ROOT/result" flake3#fnord)
mv "$flake2Dir.tmp" "$flake2Dir"
mv "$nonFlakeDir.tmp" "$nonFlakeDir"
bsd build -o "$TEST_ROOT/result" flake3#xyzzy flake3#fnord

# Check non-flake inputs have a sourceInfo and an outPath
#
# This may look redundant, but the other checks below happen in a command
# substitution subshell, so failures there will not exit this shell
bsd eval --raw flake3#inputs.nonFlake.outPath
bsd eval --raw flake3#inputs.nonFlake.sourceInfo.outPath
bsd eval --raw flake3#inputs.nonFlakeFile.outPath
bsd eval --raw flake3#inputs.nonFlakeFile.sourceInfo.outPath
bsd eval --raw flake3#inputs.nonFlakeFile2.outPath
bsd eval --raw flake3#inputs.nonFlakeFile2.sourceInfo.outPath
bsd eval --raw flake3#inputs.nonFlakeFile3.outPath
bsd eval --raw flake3#inputs.nonFlakeFile3.sourceInfo.outPath
bsd eval --raw flake3#inputs.relativeNonFlakeFile.outPath
bsd eval --raw flake3#inputs.relativeNonFlakeFile.sourceInfo.outPath

# Check non-flake file inputs have the expected outPaths
[[
  $(bsd eval --raw flake3#inputs.nonFlake.outPath) \
  = $(bsd eval --raw flake3#inputs.nonFlake.sourceInfo.outPath)
]]
[[
  $(bsd eval --raw flake3#inputs.nonFlakeFile.outPath) \
  = $(bsd eval --raw flake3#inputs.nonFlakeFile.sourceInfo.outPath)
]]
[[
  $(bsd eval --raw flake3#inputs.nonFlakeFile2.outPath) \
  = $(bsd eval --raw flake3#inputs.nonFlakeFile2.sourceInfo.outPath)
]]
[[
  $(bsd eval --raw flake3#inputs.nonFlakeFile3.outPath) \
  = $(bsd eval --raw flake3#inputs.nonFlakeFile3.sourceInfo.outPath)/README.md
]]
[[
  $(bsd eval --raw flake3#inputs.relativeNonFlakeFile.outPath) \
  = $(bsd eval --raw flake3#inputs.relativeNonFlakeFile.sourceInfo.outPath)/config.bsd
]]

# Make branch "removeXyzzy" where flake3 doesn't have xyzzy anymore
git -C "$flake3Dir" checkout -b removeXyzzy
rm "$flake3Dir/flake.bsd"

cat > "$flake3Dir/flake.bsd" <<EOF
{
  inputs = {
    nonFlake = {
      url = "$nonFlakeDir";
      flake = false;
    };
  };

  description = "Fnord";

  outputs = { self, flake1, flake2, nonFlake }: rec {
    packages.$system.sth = flake1.packages.$system.foo;
    packages.$system.fnord =
      with import ./config.bsd;
      mkDerivation {
        inherit system;
        name = "fnord";
        buildCommand = ''
          cat \${nonFlake}/README.md > \$out
        '';
      };
  };
}
EOF
bsd flake lock "$flake3Dir"
git -C "$flake3Dir" add flake.bsd flake.lock
git -C "$flake3Dir" commit -m 'Remove packages.xyzzy'
git -C "$flake3Dir" checkout master

# Test whether fuzzy-matching works for registry entries.
bsd registry add --registry "$registry" flake4 flake3
(! bsd build -o "$TEST_ROOT/result" flake4/removeXyzzy#xyzzy)
bsd build -o "$TEST_ROOT/result" flake4/removeXyzzy#sth
