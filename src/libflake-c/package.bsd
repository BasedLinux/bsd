{
  lib,
  mkMesonLibrary,

  bsd-store-c,
  bsd-expr-c,
  bsd-fetchers-c,
  bsd-flake,

  # Configuration Options

  version,
}:

let
  inherit (lib) fileset;
in

mkMesonLibrary (finalAttrs: {
  pname = "bsd-flake-c";
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
    (fileset.fileFilter (file: file.hasExt "h") ./.)
  ];

  propagatedBuildInputs = [
    bsd-expr-c
    bsd-store-c
    bsd-fetchers-c
    bsd-flake
  ];

  mesonFlags = [
  ];

  meta = {
    platforms = lib.platforms.ubsd ++ lib.platforms.windows;
  };

})
