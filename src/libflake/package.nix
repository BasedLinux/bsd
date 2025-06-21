{
  lib,
  mkMesonLibrary,

  bsd-util,
  bsd-store,
  bsd-fetchers,
  bsd-expr,
  nlohmann_json,

  # Configuration Options

  version,
}:

let
  inherit (lib) fileset;
in

mkMesonLibrary (finalAttrs: {
  pname = "bsd-flake";
  inherit version;

  workDir = ./.;
  fileset = fileset.unions [
    ../../bsd-meson-build-support
    ./bsd-meson-build-support
    ../../.version
    ./.version
    ./meson.build
    ./include/bsd/flake/meson.build
    ./call-flake.nix
    (fileset.fileFilter (file: file.hasExt "cc") ./.)
    (fileset.fileFilter (file: file.hasExt "hh") ./.)
  ];

  propagatedBuildInputs = [
    bsd-store
    bsd-util
    bsd-fetchers
    bsd-expr
    nlohmann_json
  ];

  meta = {
    platforms = lib.platforms.ubsd ++ lib.platforms.windows;
  };

})
