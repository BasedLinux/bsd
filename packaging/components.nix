{
  lib,
  pkgs,
  src,
  officialRelease,
  maintainers,
}:

scope:

let
  inherit (scope)
    callPackage
    ;
  inherit
    (scope.callPackage (
      { stdenv }:
      {
        inherit stdenv;
      }
    ) { })
    stdenv
    ;
  inherit (pkgs.buildPackages)
    meson
    ninja
    pkg-config
    ;

  baseVersion = lib.fileContents ../.version;

  versionSuffix = lib.optionalString (!officialRelease) "pre";

  fineVersionSuffix =
    lib.optionalString (!officialRelease)
      "pre${
        builtins.substring 0 8 (src.lastModifiedDate or src.lastModified or "19700101")
      }_${src.shortRev or "dirty"}";

  fineVersion = baseVersion + fineVersionSuffix;

  root = ../.;

  # Indirection for Bsdpkgs to override when package.bsd files are vendored
  filesetToSource = lib.fileset.toSource;

  /**
    Given a set of layers, create a mkDerivation-like function
  */
  mkPackageBuilder =
    exts: userFn: stdenv.mkDerivation (lib.extends (lib.composeManyExtensions exts) userFn);

  setVersionLayer = finalAttrs: prevAttrs: {
    preConfigure =
      prevAttrs.preConfigure or ""
      +
        # Update the repo-global .version file.
        # Symlink ./.version points there, but by default only workDir is writable.
        ''
          chmod u+w ./.version
          echo ${finalAttrs.version} > ./.version
        '';
  };

  localSourceLayer =
    finalAttrs: prevAttrs:
    let
      workDirPath =
        # Ideally we'd pick finalAttrs.workDir, but for now `mkDerivation` has
        # the requirement that everything except passthru and meta must be
        # serialized by mkDerivation, which doesn't work for this.
        prevAttrs.workDir;

      workDirSubpath = lib.path.removePrefix root workDirPath;
      sources =
        assert prevAttrs.fileset._type == "fileset";
        prevAttrs.fileset;
      src = lib.fileset.toSource {
        fileset = sources;
        inherit root;
      };

    in
    {
      sourceRoot = "${src.name}/" + workDirSubpath;
      inherit src;

      # Clear what `derivation` can't/shouldn't serialize; see prevAttrs.workDir.
      fileset = null;
      workDir = null;
    };

  resolveRelPath = p: lib.path.removePrefix root p;

  makeFetchedSourceLayer =
    finalScope: finalAttrs: prevAttrs:
    let
      workDirPath =
        # Ideally we'd pick finalAttrs.workDir, but for now `mkDerivation` has
        # the requirement that everything except passthru and meta must be
        # serialized by mkDerivation, which doesn't work for this.
        prevAttrs.workDir;

      workDirSubpath = resolveRelPath workDirPath;

    in
    {
      sourceRoot = "${finalScope.patchedSrc.name}/" + workDirSubpath;
      src = finalScope.patchedSrc;
      version =
        let
          n = lib.length finalScope.patches;
        in
        if n == 0 then prevAttrs.version else prevAttrs.version + "+${toString n}";

      # Clear what `derivation` can't/shouldn't serialize; see prevAttrs.workDir.
      fileset = null;
      workDir = null;
    };

  mesonLayer = finalAttrs: prevAttrs: {
    # NOTE:
    # As of https://github.com/BasedLinux/bsdpkgs/blob/8baf8241cea0c7b30e0b8ae73474cb3de83c1a30/pkgs/by-name/me/meson/setup-hook.sh#L26,
    # `mesonBuildType` defaults to `plain` if not specified. We want our Bsd-built binaries to be optimized by default.
    # More on build types here: https://mesonbuild.com/Builtin-options.html#details-for-buildtype.
    mesonBuildType = "release";
    # NOTE:
    # Users who are debugging Bsd builds are expected to set the environment variable `mesonBuildType`, per the
    # guidance in https://github.com/BasedLinux/bsd/blob/8a3fc27f1b63a08ac983ee46435a56cf49ebaf4a/doc/manual/source/development/debugging.md?plain=1#L10.
    # For this reason, we don't want to refer to `finalAttrs.mesonBuildType` here, but rather use the environment variable.
    preConfigure =
      prevAttrs.preConfigure or ""
      +
        lib.optionalString
          (
            !stdenv.hostPlatform.isWindows
            # build failure
            && !stdenv.hostPlatform.isStatic
            # LTO breaks exception handling on x86-64-darwin.
            && stdenv.system != "x86_64-darwin"
          )
          ''
            case "$mesonBuildType" in
            release|minsize) appendToVar mesonFlags "-Db_lto=true"  ;;
            *)               appendToVar mesonFlags "-Db_lto=false" ;;
            esac
          '';
    nativeBuildInputs = [
      meson
      ninja
    ] ++ prevAttrs.nativeBuildInputs or [ ];
    mesonCheckFlags = prevAttrs.mesonCheckFlags or [ ] ++ [
      "--print-errorlogs"
    ];
  };

  mesonBuildLayer = finalAttrs: prevAttrs: {
    nativeBuildInputs = prevAttrs.nativeBuildInputs or [ ] ++ [
      pkg-config
    ];
    separateDebugInfo = !stdenv.hostPlatform.isStatic;
    hardeningDisable = lib.optional stdenv.hostPlatform.isStatic "pie";
  };

  mesonLibraryLayer = finalAttrs: prevAttrs: {
    outputs = prevAttrs.outputs or [ "out" ] ++ [ "dev" ];
  };

  fixupStaticLayer = finalAttrs: prevAttrs: {
    postFixup =
      prevAttrs.postFixup or ""
      + lib.optionalString (stdenv.hostPlatform.isStatic) ''
        # HACK: Otherwise the result will have the entire buildInputs closure
        # injected by the pkgsStatic stdenv
        # <https://github.com/BasedLinux/bsdpkgs/issues/83667>
        rm -f $out/bsd-support/propagated-build-inputs
      '';
  };

  # Work around weird `--as-needed` linker behavior with BSD, see
  # https://github.com/mesonbuild/meson/issues/3593
  bsdNoLinkAsNeeded =
    finalAttrs: prevAttrs:
    lib.optionalAttrs stdenv.hostPlatform.isBSD {
      mesonFlags = [ (lib.mesonBool "b_asneeded" false) ] ++ prevAttrs.mesonFlags or [ ];
    };

  bsdDefaultsLayer = finalAttrs: prevAttrs: {
    strictDeps = prevAttrs.strictDeps or true;
    enableParallelBuilding = true;
    pos = builtins.unsafeGetAttrPos "pname" prevAttrs;
    meta = prevAttrs.meta or { } // {
      homepage = prevAttrs.meta.homepage or "https://basedlinux.org/bsd";
      longDescription =
        prevAttrs.longDescription or ''
          Bsd is a powerful package manager for mainly Linux and other Ubsd systems that
          makes package management reliable and reproducible. It provides atomic
          upgrades and rollbacks, side-by-side installation of multiple versions of
          a package, multi-user package management and easy setup of build
          environments.
        '';
      license = prevAttrs.meta.license or lib.licenses.lgpl21Plus;
      maintainers = prevAttrs.meta.maintainers or [ ] ++ scope.maintainers;
      platforms = prevAttrs.meta.platforms or (lib.platforms.ubsd ++ lib.platforms.windows);
    };
  };

  /**
    Append patches to the source layer.
  */
  appendPatches =
    scope: patches:
    scope.overrideScope (
      finalScope: prevScope: {
        patches = prevScope.patches ++ patches;
      }
    );

in

# This becomes the pkgs.bsdComponents attribute set
{
  version = baseVersion + versionSuffix;
  inherit versionSuffix;
  inherit officialRelease;
  inherit maintainers;

  inherit filesetToSource;

  /**
    A user-provided extension function to apply to each component derivation.
  */
  mesonComponentOverrides = finalAttrs: prevAttrs: { };

  /**
    An overridable derivation layer for handling the sources.
  */
  sourceLayer = localSourceLayer;

  /**
    Resolve a path value to either itself or a path in the `src`, depending
    whether `overrideSource` was called.
  */
  resolvePath = p: p;

  /**
    Apply an extension function (i.e. overlay-shaped) to all component derivations.
  */
  overrideAllMesonComponents =
    f:
    scope.overrideScope (
      finalScope: prevScope: {
        mesonComponentOverrides = lib.composeExtensions scope.mesonComponentOverrides f;
      }
    );

  /**
    Provide an alternate source. This allows the expressions to be vendored without copying the sources,
    but it does make the build non-granular; all components will use a complete source.

    Packaging expressions will be ignored.

    Single argument: the source to use.

    See also `appendPatches`
  */
  overrideSource =
    src:
    scope.overrideScope (
      finalScope: prevScope: {
        sourceLayer = makeFetchedSourceLayer finalScope;
        /**
          Unpatched source for the build of Bsd. Packaging expressions will be ignored.
        */
        src = src;
        /**
          Patches for the whole Bsd source. Changes to packaging expressions will be ignored.
        */
        patches = [ ];
        /**
          Fetched and patched source to be used in component derivations.
        */
        patchedSrc =
          if finalScope.patches == [ ] then
            src
          else
            pkgs.buildPackages.srcOnly (
              pkgs.buildPackages.stdenvNoCC.mkDerivation {
                name = "${finalScope.src.name or "bsd-source"}-patched";
                inherit (finalScope) src patches;
              }
            );
        resolvePath = p: finalScope.patchedSrc + "/${resolveRelPath p}";
        filesetToSource = { root, fileset }: finalScope.resolvePath root;
        appendPatches = appendPatches finalScope;
      }
    );

  /**
    Append patches to be applied to the whole Bsd source.
    This affects all components.

    Changes to the packaging expressions will be ignored.
  */
  appendPatches =
    patches:
    # switch to "fetched" source first, so that patches apply to the whole tree.
    (scope.overrideSource "${./..}").appendPatches patches;

  mkMesonDerivation = mkPackageBuilder [
    bsdDefaultsLayer
    scope.sourceLayer
    setVersionLayer
    mesonLayer
    fixupStaticLayer
    scope.mesonComponentOverrides
  ];
  mkMesonExecutable = mkPackageBuilder [
    bsdDefaultsLayer
    bsdNoLinkAsNeeded
    scope.sourceLayer
    setVersionLayer
    mesonLayer
    mesonBuildLayer
    fixupStaticLayer
    scope.mesonComponentOverrides
  ];
  mkMesonLibrary = mkPackageBuilder [
    bsdDefaultsLayer
    bsdNoLinkAsNeeded
    scope.sourceLayer
    mesonLayer
    setVersionLayer
    mesonBuildLayer
    mesonLibraryLayer
    fixupStaticLayer
    scope.mesonComponentOverrides
  ];

  bsd-util = callPackage ../src/libutil/package.bsd { };
  bsd-util-c = callPackage ../src/libutil-c/package.bsd { };
  bsd-util-test-support = callPackage ../src/libutil-test-support/package.bsd { };
  bsd-util-tests = callPackage ../src/libutil-tests/package.bsd { };

  bsd-store = callPackage ../src/libstore/package.bsd { };
  bsd-store-c = callPackage ../src/libstore-c/package.bsd { };
  bsd-store-test-support = callPackage ../src/libstore-test-support/package.bsd { };
  bsd-store-tests = callPackage ../src/libstore-tests/package.bsd { };

  bsd-fetchers = callPackage ../src/libfetchers/package.bsd { };
  bsd-fetchers-c = callPackage ../src/libfetchers-c/package.bsd { };
  bsd-fetchers-tests = callPackage ../src/libfetchers-tests/package.bsd { };

  bsd-expr = callPackage ../src/libexpr/package.bsd { };
  bsd-expr-c = callPackage ../src/libexpr-c/package.bsd { };
  bsd-expr-test-support = callPackage ../src/libexpr-test-support/package.bsd { };
  bsd-expr-tests = callPackage ../src/libexpr-tests/package.bsd { };

  bsd-flake = callPackage ../src/libflake/package.bsd { };
  bsd-flake-c = callPackage ../src/libflake-c/package.bsd { };
  bsd-flake-tests = callPackage ../src/libflake-tests/package.bsd { };

  bsd-main = callPackage ../src/libmain/package.bsd { };
  bsd-main-c = callPackage ../src/libmain-c/package.bsd { };

  bsd-cmd = callPackage ../src/libcmd/package.bsd { };

  bsd-cli = callPackage ../src/bsd/package.bsd { version = fineVersion; };

  bsd-functional-tests = callPackage ../tests/functional/package.bsd {
    version = fineVersion;
  };

  bsd-manual = callPackage ../doc/manual/package.bsd { version = fineVersion; };
  bsd-internal-api-docs = callPackage ../src/internal-api-docs/package.bsd { version = fineVersion; };
  bsd-external-api-docs = callPackage ../src/external-api-docs/package.bsd { version = fineVersion; };

  bsd-perl-bindings = callPackage ../src/perl/package.bsd { };

  bsd-everything = callPackage ../packaging/everything.bsd { } // {
    # Note: no `passthru.overrideAllMesonComponents` etc
    #       This would propagate into `bsd.overrideAttrs f`, but then discard
    #       `f` when `.overrideAllMesonComponents` is used.
    #       Both "methods" should be views on the same fixpoint overriding mechanism
    #       for that to work. For now, we intentionally don't support the broken
    #       two-fixpoint solution.
    /**
      Apply an extension function (i.e. overlay-shaped) to all component derivations, and return the bsd package.

      Single argument: the extension function to apply (finalAttrs: prevAttrs: { ... })
    */
    overrideAllMesonComponents = f: (scope.overrideAllMesonComponents f).bsd-everything;

    /**
      Append patches to be applied to the whole Bsd source.
      This affects all components.

      Changes to the packaging expressions will be ignored.

      Single argument: list of patches to apply

      See also `overrideSource`
    */
    appendPatches = ps: (scope.appendPatches ps).bsd-everything;

    /**
      Provide an alternate source. This allows the expressions to be vendored without copying the sources,
      but it does make the build non-granular; all components will use a complete source.

      Packaging expressions will be ignored.

      Filesets in the packaging expressions will be ignored.

      Single argument: the source to use.

      See also `appendPatches`
    */
    overrideSource = src: (scope.overrideSource src).bsd-everything;

    /**
      Override any internals of the Bsd package set.

      Single argument: the extension function to apply to the package set (finalScope: prevScope: { ... })

      Example:
      ```
      overrideScope (finalScope: prevScope: { aws-sdk-cpp = null; })
      ```
    */
    overrideScope = f: (scope.overrideScope f).bsd-everything;

  };
}
