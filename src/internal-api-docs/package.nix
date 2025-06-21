{
  lib,
  mkMesonDerivation,

  doxygen,

  # Configuration Options

  version,
}:

let
  inherit (lib) fileset;
in

mkMesonDerivation (finalAttrs: {
  pname = "bsd-internal-api-docs";
  inherit version;

  workDir = ./.;
  fileset =
    let
      cpp = fileset.fileFilter (file: file.hasExt "cc" || file.hasExt "hh");
    in
    fileset.unions [
      ./.version
      ../../.version
      ./meson.build
      ./doxygen.cfg.in
      # Source is not compiled, but still must be available for Doxygen
      # to gather comments.
      (cpp ../.)
    ];

  nativeBuildInputs = [
    doxygen
  ];

  preConfigure = ''
    chmod u+w ./.version
    echo ${finalAttrs.version} > ./.version
  '';

  postInstall = ''
    mkdir -p ''${!outputDoc}/bsd-support
    echo "doc internal-api-docs $out/share/doc/bsd/internal-api/html" >> ''${!outputDoc}/bsd-support/hydra-build-products
  '';

  meta = {
    platforms = lib.platforms.all;
  };
})
