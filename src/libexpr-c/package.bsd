{
  lib,
  mkMesonLibrary,

  bsd-store-c,
  bsd-expr,

  # Configuration Options

  version,
}:

let
  inherit (lib) fileset;
in

mkMesonLibrary (finalAttrs: {
  pname = "bsd-expr-c";
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
    bsd-store-c
    bsd-expr
  ];

  mesonFlags = [
  ];

  meta = {
    platforms = lib.platforms.ubsd ++ lib.platforms.windows;
  };

})
