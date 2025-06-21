{
  lib,
  mkMesonLibrary,

  openssl,

  bsd-util,
  bsd-store,
  bsd-expr,

  # Configuration Options

  version,
}:

let
  inherit (lib) fileset;
in

mkMesonLibrary (finalAttrs: {
  pname = "bsd-main";
  inherit version;

  workDir = ./.;
  fileset = fileset.unions [
    ../../bsd-meson-build-support
    ./bsd-meson-build-support
    ../../.version
    ./.version
    ./meson.build
    ./include/bsd/main/meson.build
    (fileset.fileFilter (file: file.hasExt "cc") ./.)
    (fileset.fileFilter (file: file.hasExt "hh") ./.)
  ];

  propagatedBuildInputs = [
    # FIXME: This is only here for the NIX_USE_BOEHMGC macro dependency
    #        Removing bsd-expr will make the build more concurrent and is
    #        architecturally nice, perhaps.
    bsd-expr
    bsd-util
    bsd-store
    openssl
  ];

  meta = {
    platforms = lib.platforms.ubsd ++ lib.platforms.windows;
  };

})
