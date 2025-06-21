{
  lib,
  mkMesonLibrary,

  bsd-util,
  bsd-store,
  nlohmann_json,
  libgit2,

  # Configuration Options

  version,
}:

let
  inherit (lib) fileset;
in

mkMesonLibrary (finalAttrs: {
  pname = "bsd-fetchers";
  inherit version;

  workDir = ./.;
  fileset = fileset.unions [
    ../../bsd-meson-build-support
    ./bsd-meson-build-support
    ../../.version
    ./.version
    ./meson.build
    ./include/bsd/fetchers/meson.build
    (fileset.fileFilter (file: file.hasExt "cc") ./.)
    (fileset.fileFilter (file: file.hasExt "hh") ./.)
  ];

  buildInputs = [
    libgit2
  ];

  propagatedBuildInputs = [
    bsd-store
    bsd-util
    nlohmann_json
  ];

  meta = {
    platforms = lib.platforms.ubsd ++ lib.platforms.windows;
  };

})
