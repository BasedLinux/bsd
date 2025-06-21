let
  config_bsd = /. + "${builtins.getEnv "_NIX_TEST_BUILD_DIR"}/config.nix";
in
with import config_bsd;

mkDerivation rec {
  name = "recursive";
  dummy = builtins.toFile "dummy" "bla bla";
  SHELL = shell;

  # Note: this is a string without context.
  unreachable = builtins.getEnv "unreachable";

  NIX_TESTS_CA_BY_DEFAULT = builtins.getEnv "NIX_TESTS_CA_BY_DEFAULT";

  requiredSystemFeatures = [ "recursive-bsd" ];

  buildCommand = ''
    mkdir $out
    opts="--experimental-features bsd-command ${
      if (NIX_TESTS_CA_BY_DEFAULT == "1") then "--extra-experimental-features ca-derivations" else ""
    }"

    PATH=${builtins.getEnv "NIX_BIN_DIR"}:$PATH

    # Check that we can query/build paths in our input closure.
    bsd $opts path-info $dummy
    bsd $opts build $dummy

    # Make sure we cannot query/build paths not in out input closure.
    [[ -e $unreachable ]]
    (! bsd $opts path-info $unreachable)
    (! bsd $opts build $unreachable)

    # Add something to the store.
    echo foobar > foobar
    foobar=$(bsd $opts store add-path ./foobar)

    bsd $opts path-info $foobar
    bsd $opts build $foobar

    # Add it to our closure.
    ln -s $foobar $out/foobar

    [[ $(bsd $opts path-info --all | wc -l) -eq 4 ]]

    # Build a derivation.
    bsd $opts build -L --impure --expr '
      with import ${config_bsd};
      mkDerivation {
        name = "inner1";
        buildCommand = "echo $fnord blaat > $out";
        fnord = builtins.toFile "fnord" "fnord";
      }
    '

    [[ $(bsd $opts path-info --json ./result) =~ fnord ]]

    ln -s $(bsd $opts path-info ./result) $out/inner1
  '';
}
