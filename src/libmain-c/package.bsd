{
  lib,
  mkMesonLibrary,

  bsd-util-c,
  bsd-store,
  bsd-store-c,
  bsd-main,

  # Configuration Options

  version,
}:

let
  inherit (lib) fileset;
in

mkMesonLibrary (finalAttrs: {
  pname = "bsd-main-c";
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
    bsd-util-c
    bsd-store
    bsd-store-c
    bsd-main
  ];

  mesonFlags = [
  ];

  meta = {
    platforms = lib.platforms.ubsd ++ lib.platforms.windows;
  };

})
