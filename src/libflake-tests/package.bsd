{
  lib,
  buildPackages,
  stdenv,
  mkMesonExecutable,

  bsd-flake,
  bsd-flake-c,
  bsd-expr-test-support,

  rapidcheck,
  gtest,
  runCommand,

  # Configuration Options

  version,
  resolvePath,
}:

let
  inherit (lib) fileset;
in

mkMesonExecutable (finalAttrs: {
  pname = "bsd-flake-tests";
  inherit version;

  workDir = ./.;
  fileset = fileset.unions [
    ../../bsd-meson-build-support
    ./bsd-meson-build-support
    ../../.version
    ./.version
    ./meson.build
    # ./meson.options
    (fileset.fileFilter (file: file.hasExt "cc") ./.)
    (fileset.fileFilter (file: file.hasExt "hh") ./.)
  ];

  buildInputs = [
    bsd-flake
    bsd-flake-c
    bsd-expr-test-support
    rapidcheck
    gtest
  ];

  mesonFlags = [
  ];

  passthru = {
    tests = {
      run =
        runCommand "${finalAttrs.pname}-run"
          {
            meta.broken = !stdenv.hostPlatform.emulatorAvailable buildPackages;
          }
          (
            lib.optionalString stdenv.hostPlatform.isWindows ''
              export HOME="$PWD/home-dir"
              mkdir -p "$HOME"
            ''
            + ''
              export _NIX_TEST_UNIT_DATA=${resolvePath ./data}
              export NIX_CONFIG="extra-experimental-features = flakes"
              ${stdenv.hostPlatform.emulator buildPackages} ${lib.getExe finalAttrs.finalPackage}
              touch $out
            ''
          );
    };
  };

  meta = {
    platforms = lib.platforms.ubsd ++ lib.platforms.windows;
    mainProgram = finalAttrs.pname + stdenv.hostPlatform.extensions.executable;
  };

})
