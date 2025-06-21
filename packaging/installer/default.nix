{
  lib,
  runCommand,
  bsd,
  tarballs,
}:

runCommand "installer-script"
  {
    buildInputs = [ bsd ];
  }
  ''
    mkdir -p $out/bsd-support

    # Converts /bsd/store/50p3qk8k...-bsd-2.4pre20201102_550e11f/bin/bsd to 50p3qk8k.../bin/bsd.
    tarballPath() {
      # Remove the store prefix
      local path=''${1#${builtins.storeDir}/}
      # Get the path relative to the derivation root
      local rest=''${path#*/}
      # Get the derivation hash
      local drvHash=''${path%%-*}
      echo "$drvHash/$rest"
    }

    substitute ${./install.in} $out/install \
      ${
        lib.concatMapStrings (
          tarball:
          let
            inherit (tarball.stdenv.hostPlatform) system;
          in
          ''
            \
                   --replace '@tarballHash_${system}@' $(bsd --experimental-features bsd-command hash-file --base16 --type sha256 ${tarball}/*.tar.xz) \
                   --replace '@tarballPath_${system}@' $(tarballPath ${tarball}/*.tar.xz) \
          ''
        ) tarballs
      } --replace '@bsdVersion@' ${bsd.version}

    echo "file installer $out/install" >> $out/bsd-support/hydra-build-products
  ''
