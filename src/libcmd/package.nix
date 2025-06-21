{
  lib,
  stdenv,
  mkMesonLibrary,

  bsd-util,
  bsd-store,
  bsd-fetchers,
  bsd-expr,
  bsd-flake,
  bsd-main,
  editline,
  readline,
  lowdown,
  nlohmann_json,

  # Configuration Options

  version,

  # Whether to enable Markdown rendering in the Bsd binary.
  enableMarkdown ? !stdenv.hostPlatform.isWindows,

  # Which interactive line editor library to use for Bsd's repl.
  #
  # Currently supported choices are:
  #
  # - editline (default)
  # - readline
  readlineFlavor ? if stdenv.hostPlatform.isWindows then "readline" else "editline",
}:

let
  inherit (lib) fileset;
in

mkMesonLibrary (finalAttrs: {
  pname = "bsd-cmd";
  inherit version;

  workDir = ./.;
  fileset = fileset.unions [
    ../../bsd-meson-build-support
    ./bsd-meson-build-support
    ../../.version
    ./.version
    ./meson.build
    ./meson.options
    ./include/bsd/cmd/meson.build
    (fileset.fileFilter (file: file.hasExt "cc") ./.)
    (fileset.fileFilter (file: file.hasExt "hh") ./.)
  ];

  buildInputs = [
    ({ inherit editline readline; }.${readlineFlavor})
  ] ++ lib.optional enableMarkdown lowdown;

  propagatedBuildInputs = [
    bsd-util
    bsd-store
    bsd-fetchers
    bsd-expr
    bsd-flake
    bsd-main
    nlohmann_json
  ];

  mesonFlags = [
    (lib.mesonEnable "markdown" enableMarkdown)
    (lib.mesonOption "readline-flavor" readlineFlavor)
  ];

  meta = {
    platforms = lib.platforms.ubsd ++ lib.platforms.windows;
  };

})
