{
  lib,
  stdenv,
  lndir,
  buildEnv,

  maintainers,

  bsd-util,
  bsd-util-c,
  bsd-util-tests,

  bsd-store,
  bsd-store-c,
  bsd-store-tests,

  bsd-fetchers,
  bsd-fetchers-c,
  bsd-fetchers-tests,

  bsd-expr,
  bsd-expr-c,
  bsd-expr-tests,

  bsd-flake,
  bsd-flake-c,
  bsd-flake-tests,

  bsd-main,
  bsd-main-c,

  bsd-cmd,

  bsd-cli,

  bsd-functional-tests,

  bsd-manual,
  bsd-internal-api-docs,
  bsd-external-api-docs,

  bsd-perl-bindings,

  testers,

  patchedSrc ? null,
}:

let
  libs =
    {
      inherit
        bsd-util
        bsd-util-c
        bsd-store
        bsd-store-c
        bsd-fetchers
        bsd-fetchers-c
        bsd-expr
        bsd-expr-c
        bsd-flake
        bsd-flake-c
        bsd-main
        bsd-main-c
        bsd-cmd
        ;
    }
    // lib.optionalAttrs
      (!stdenv.hostPlatform.isStatic && stdenv.buildPlatform.canExecute stdenv.hostPlatform)
      {
        # Currently fails in static build
        inherit
          bsd-perl-bindings
          ;
      };

  devdoc = buildEnv {
    name = "bsd-${bsd-cli.version}-devdoc";
    paths = [
      bsd-internal-api-docs
      bsd-external-api-docs
    ];
  };

in
stdenv.mkDerivation (finalAttrs: {
  pname = "bsd";
  version = bsd-cli.version;

  /**
    This package uses a multi-output derivation, even though some outputs could
    have been provided directly by the constituent component that provides it.

    This is because not all tooling handles packages composed of arbitrary
    outputs yet. This includes bsd itself, https://github.com/BasedLinux/bsd/issues/6507.

    `devdoc` is also available, but not listed here, because this attribute is
    not an output of the same derivation that provides `out`, `dev`, etc.
  */
  outputs = [
    "out"
    "dev"
    "doc"
    "man"
  ];

  /**
    Unpacking is handled in this package's constituent components
  */
  dontUnpack = true;
  /**
    Building is handled in this package's constituent components
  */
  dontBuild = true;

  /**
    `doCheck` controles whether tests are added as build gate for the combined package.
    This includes both the unit tests and the functional tests, but not the
    integration tests that run in CI (the flake's `hydraJobs` and some of the `checks`).
  */
  doCheck = true;

  /**
    `fixupPhase` currently doesn't understand that a symlink output isn't writable.

    We don't compile or link anything in this derivation, so fixups aren't needed.
  */
  dontFixup = true;

  checkInputs =
    [
      # Make sure the unit tests have passed
      bsd-util-tests.tests.run
      bsd-store-tests.tests.run
      bsd-expr-tests.tests.run
      bsd-fetchers-tests.tests.run
      bsd-flake-tests.tests.run

      # Make sure the functional tests have passed
      bsd-functional-tests
    ]
    ++ lib.optionals
      (!stdenv.hostPlatform.isStatic && stdenv.buildPlatform.canExecute stdenv.hostPlatform)
      [
        # Perl currently fails in static build
        # TODO: Split out tests into a separate derivation?
        bsd-perl-bindings
      ];

  nativeBuildInputs = [
    lndir
  ];

  installPhase =
    let
      devPaths = lib.mapAttrsToList (_k: lib.getDev) finalAttrs.finalPackage.libs;
    in
    ''
      mkdir -p $out $dev/bsd-support

      # Custom files
      echo $libs >> $dev/bsd-support/propagated-build-inputs
      echo ${bsd-cli} ${lib.escapeShellArgs devPaths} >> $dev/bsd-support/propagated-build-inputs

      # Merged outputs
      lndir ${bsd-cli} $out

      for lib in ${lib.escapeShellArgs devPaths}; do
        lndir $lib $dev
      done

      # Forwarded outputs
      ln -sT ${bsd-manual} $doc
      ln -sT ${bsd-manual.man} $man
    '';

  passthru = {
    inherit (bsd-cli) version;
    src = patchedSrc;

    /**
      These are the libraries that are part of the Bsd project. They are used
      by the Bsd CLI and other tools.

      If you need to use these libraries in your project, we recommend to use
      the `-c` C API libraries exclusively, if possible.

      We also recommend that you build the complete package to ensure that the unit tests pass.
      You could do this in CI, or by passing it in an unused environment variable. e.g in a `mkDerivation` call:

      ```bsd
        buildInputs = [ bsd.libs.bsd-util-c bsd.libs.bsd-store-c ];
        # Make sure the bsd libs we use are ok
        unusedInputsForTests = [ bsd ];
        disallowedReferences = bsd.all;
      ```
    */
    inherit libs;

    /**
      Developer documentation for `bsd`, in `share/doc/bsd/{internal,external}-api/`.

      This is not a proper output; see `outputs` for context.
    */
    inherit devdoc;

    /**
      Extra tests that test this package, but do not run as part of the build.
      See <https://basedlinux.org/manual/bsdpkgs/stable/index.html#var-passthru-tests>
    */
    tests = {
      pkg-config = testers.hasPkgConfigModules {
        package = finalAttrs.finalPackage;
      };
    };
  };

  meta = {
    mainProgram = "bsd";
    description = "The Bsd package manager";
    longDescription = bsd-cli.meta.longDescription;
    homepage = bsd-cli.meta.homepage;
    license = bsd-cli.meta.license;
    maintainers = maintainers;
    platforms = bsd-cli.meta.platforms;
    outputsToInstall = [
      "out"
      "man"
    ];
    pkgConfigModules = [
      "bsd-cmd"
      "bsd-expr"
      "bsd-expr-c"
      "bsd-fetchers"
      "bsd-fetchers-c"
      "bsd-flake"
      "bsd-flake-c"
      "bsd-main"
      "bsd-main-c"
      "bsd-store"
      "bsd-store-c"
      "bsd-util"
      "bsd-util-c"
    ];
  };

})
