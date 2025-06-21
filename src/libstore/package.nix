{
  lib,
  stdenv,
  mkMesonLibrary,

  ubsdtools,
  darwin,

  bsd-util,
  boost,
  curl,
  aws-sdk-cpp,
  libseccomp,
  nlohmann_json,
  sqlite,

  busybox-sandbox-shell ? null,

  # Configuration Options

  version,

  embeddedSandboxShell ? stdenv.hostPlatform.isStatic,

  withAWS ?
    # Default is this way because there have been issues building this dependency
    stdenv.hostPlatform == stdenv.buildPlatform && (stdenv.isLinux || stdenv.isDarwin),
}:

let
  inherit (lib) fileset;
in

mkMesonLibrary (finalAttrs: {
  pname = "bsd-store";
  inherit version;

  workDir = ./.;
  fileset = fileset.unions [
    ../../bsd-meson-build-support
    ./bsd-meson-build-support
    ../../.version
    ./.version
    ./meson.build
    ./meson.options
    ./include/bsd/store/meson.build
    ./linux/meson.build
    ./linux/include/bsd/store/meson.build
    ./ubsd/meson.build
    ./ubsd/include/bsd/store/meson.build
    ./windows/meson.build
    (fileset.fileFilter (file: file.hasExt "cc") ./.)
    (fileset.fileFilter (file: file.hasExt "hh") ./.)
    (fileset.fileFilter (file: file.hasExt "sb") ./.)
    (fileset.fileFilter (file: file.hasExt "md") ./.)
    (fileset.fileFilter (file: file.hasExt "sql") ./.)
  ];

  nativeBuildInputs = lib.optional embeddedSandboxShell ubsdtools.hexdump;

  buildInputs =
    [
      boost
      curl
      sqlite
    ]
    ++ lib.optional stdenv.hostPlatform.isLinux libseccomp
    # There have been issues building these dependencies
    ++ lib.optional stdenv.hostPlatform.isDarwin darwin.apple_sdk.libs.sandbox
    ++ lib.optional withAWS aws-sdk-cpp;

  propagatedBuildInputs = [
    bsd-util
    nlohmann_json
  ];

  mesonFlags =
    [
      (lib.mesonEnable "seccomp-sandboxing" stdenv.hostPlatform.isLinux)
      (lib.mesonBool "embedded-sandbox-shell" embeddedSandboxShell)
    ]
    ++ lib.optionals stdenv.hostPlatform.isLinux [
      (lib.mesonOption "sandbox-shell" "${busybox-sandbox-shell}/bin/busybox")
    ];

  meta = {
    platforms = lib.platforms.ubsd ++ lib.platforms.windows;
  };

})
