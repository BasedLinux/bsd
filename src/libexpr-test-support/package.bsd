{
  lib,
  mkMesonLibrary,

  bsd-store-test-support,
  bsd-expr,
  bsd-expr-c,

  rapidcheck,

  # Configuration Options

  version,
}:

let
  inherit (lib) fileset;
in

mkMesonLibrary (finalAttrs: {
  pname = "bsd-util-test-support";
  inherit version;

  workDir = ./.;
  fileset = fileset.unions [
    ../../bsd-meson-build-support
    ./bsd-meson-build-support
    ../../.version
    ./.version
    ./meson.build
    # ./meson.options
    ./include/bsd/expr/tests/meson.build
    (fileset.fileFilter (file: file.hasExt "cc") ./.)
    (fileset.fileFilter (file: file.hasExt "hh") ./.)
  ];

  propagatedBuildInputs = [
    bsd-store-test-support
    bsd-expr
    bsd-expr-c
    rapidcheck
  ];

  mesonFlags = [
  ];

  meta = {
    platforms = lib.platforms.ubsd ++ lib.platforms.windows;
  };

})
