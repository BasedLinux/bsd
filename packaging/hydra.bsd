{
  inputs,
  forAllCrossSystems,
  forAllSystems,
  lib,
  linux64BitSystems,
  bsdpkgsFor,
  self,
  officialRelease,
}:
let
  inherit (inputs) bsdpkgs bsdpkgs-regression;

  installScriptFor =
    tarballs:
    bsdpkgsFor.x86_64-linux.native.callPackage ./installer {
      inherit tarballs;
    };

  testBsdVersions =
    pkgs: daemon:
    pkgs.bsdComponents2.bsd-functional-tests.override {
      pname = "bsd-daemon-compat-tests";
      version = "${pkgs.bsd.version}-with-daemon-${daemon.version}";

      test-daemon = daemon;
    };

  # Technically we could just return `pkgs.bsdComponents2`, but for Hydra it's
  # convention to transpose it, and to transpose it efficiently, we need to
  # enumerate them manually, so that we don't evaluate unnecessary package sets.
  # See listingIsComplete below.
  forAllPackages = forAllPackages' { };
  forAllPackages' =
    {
      enableBindings ? false,
      enableDocs ? false, # already have separate attrs for these
    }:
    lib.genAttrs (
      [
        "bsd-everything"
        "bsd-util"
        "bsd-util-c"
        "bsd-util-test-support"
        "bsd-util-tests"
        "bsd-store"
        "bsd-store-c"
        "bsd-store-test-support"
        "bsd-store-tests"
        "bsd-fetchers"
        "bsd-fetchers-c"
        "bsd-fetchers-tests"
        "bsd-expr"
        "bsd-expr-c"
        "bsd-expr-test-support"
        "bsd-expr-tests"
        "bsd-flake"
        "bsd-flake-c"
        "bsd-flake-tests"
        "bsd-main"
        "bsd-main-c"
        "bsd-cmd"
        "bsd-cli"
        "bsd-functional-tests"
      ]
      ++ lib.optionals enableBindings [
        "bsd-perl-bindings"
      ]
      ++ lib.optionals enableDocs [
        "bsd-manual"
        "bsd-internal-api-docs"
        "bsd-external-api-docs"
      ]
    );
in
{
  /**
    An internal check to make sure our package listing is complete.
  */
  listingIsComplete =
    let
      arbitrarySystem = "x86_64-linux";
      listedPkgs = forAllPackages' {
        enableBindings = true;
        enableDocs = true;
      } (_: null);
      actualPkgs = lib.concatMapAttrs (
        k: v: if lib.strings.hasPrefix "bsd-" k then { ${k} = null; } else { }
      ) bsdpkgsFor.${arbitrarySystem}.native.bsdComponents2;
      diff = lib.concatStringsSep "\n" (
        lib.concatLists (
          lib.mapAttrsToList (
            k: _:
            if (listedPkgs ? ${k}) && !(actualPkgs ? ${k}) then
              [ "- ${k}: redundant?" ]
            else if !(listedPkgs ? ${k}) && (actualPkgs ? ${k}) then
              [ "- ${k}: missing?" ]
            else
              [ ]
          ) (listedPkgs // actualPkgs)
        )
      );
    in
    if listedPkgs == actualPkgs then
      { }
    else
      throw ''
        Please update the components list in hydra.bsd (or fix this check)
        Differences:
        ${diff}
      '';

  # Binary package for various platforms.
  build = forAllPackages (
    pkgName: forAllSystems (system: bsdpkgsFor.${system}.native.bsdComponents2.${pkgName})
  );

  shellInputs = removeAttrs (forAllSystems (
    system: self.devShells.${system}.default.inputDerivation
  )) [ "i686-linux" ];

  buildStatic = forAllPackages (
    pkgName:
    lib.genAttrs linux64BitSystems (
      system: bsdpkgsFor.${system}.native.pkgsStatic.bsdComponents2.${pkgName}
    )
  );

  buildCross = forAllPackages (
    pkgName:
    # Hack to avoid non-evaling package
    (
      if pkgName == "bsd-functional-tests" then
        lib.flip builtins.removeAttrs [ "x86_64-w64-mingw32" ]
      else
        lib.id
    )
      (
        forAllCrossSystems (
          crossSystem:
          lib.genAttrs [ "x86_64-linux" ] (
            system: bsdpkgsFor.${system}.cross.${crossSystem}.bsdComponents2.${pkgName}
          )
        )
      )
  );

  buildNoGc =
    let
      components = forAllSystems (
        system:
        bsdpkgsFor.${system}.native.bsdComponents2.overrideScope (
          self: super: {
            bsd-expr = super.bsd-expr.override { enableGC = false; };
          }
        )
      );
    in
    forAllPackages (pkgName: forAllSystems (system: components.${system}.${pkgName}));

  buildNoTests = forAllSystems (system: bsdpkgsFor.${system}.native.bsdComponents2.bsd-cli);

  # Toggles some settings for better coverage. Windows needs these
  # library combinations, and Debian build Bsd with GNU readline too.
  buildReadlineNoMarkdown =
    let
      components = forAllSystems (
        system:
        bsdpkgsFor.${system}.native.bsdComponents2.overrideScope (
          self: super: {
            bsd-cmd = super.bsd-cmd.override {
              enableMarkdown = false;
              readlineFlavor = "readline";
            };
          }
        )
      );
    in
    forAllPackages (pkgName: forAllSystems (system: components.${system}.${pkgName}));

  # Perl bindings for various platforms.
  perlBindings = forAllSystems (system: bsdpkgsFor.${system}.native.bsdComponents2.bsd-perl-bindings);

  # Binary tarball for various platforms, containing a Bsd store
  # with the closure of 'bsd' package, and the second half of
  # the installation script.
  binaryTarball = forAllSystems (
    system: bsdpkgsFor.${system}.native.callPackage ./binary-tarball.bsd { }
  );

  binaryTarballCross = lib.genAttrs [ "x86_64-linux" ] (
    system:
    forAllCrossSystems (
      crossSystem: bsdpkgsFor.${system}.cross.${crossSystem}.callPackage ./binary-tarball.bsd { }
    )
  );

  # The first half of the installation script. This is uploaded
  # to https://basedlinux.org/bsd/install. It downloads the binary
  # tarball for the user's system and calls the second half of the
  # installation script.
  installerScript = installScriptFor [
    # Native
    self.hydraJobs.binaryTarball."x86_64-linux"
    self.hydraJobs.binaryTarball."i686-linux"
    self.hydraJobs.binaryTarball."aarch64-linux"
    self.hydraJobs.binaryTarball."x86_64-darwin"
    self.hydraJobs.binaryTarball."aarch64-darwin"
    # Cross
    self.hydraJobs.binaryTarballCross."x86_64-linux"."armv6l-unknown-linux-gnueabihf"
    self.hydraJobs.binaryTarballCross."x86_64-linux"."armv7l-unknown-linux-gnueabihf"
    self.hydraJobs.binaryTarballCross."x86_64-linux"."riscv64-unknown-linux-gnu"
  ];

  installerScriptForGHA = forAllSystems (
    system:
    bsdpkgsFor.${system}.native.callPackage ./installer {
      tarballs = [ self.hydraJobs.binaryTarball.${system} ];
    }
  );

  # docker image with Bsd inside
  dockerImage = lib.genAttrs linux64BitSystems (system: self.packages.${system}.dockerImage);

  # # Line coverage analysis.
  # coverage = bsdpkgsFor.x86_64-linux.native.bsd.override {
  #   pname = "bsd-coverage";
  #   withCoverageChecks = true;
  # };

  # Bsd's manual
  manual = bsdpkgsFor.x86_64-linux.native.bsdComponents2.bsd-manual;

  # API docs for Bsd's unstable internal C++ interfaces.
  internal-api-docs = bsdpkgsFor.x86_64-linux.native.bsdComponents2.bsd-internal-api-docs;

  # API docs for Bsd's C bindings.
  external-api-docs = bsdpkgsFor.x86_64-linux.native.bsdComponents2.bsd-external-api-docs;

  # System tests.
  tests =
    import ../tests/bsdos {
      inherit lib bsdpkgs bsdpkgsFor;
      inherit (self.inputs) bsdpkgs-23-11;
    }
    // {

      # Make sure that bsd-env still produces the exact same result
      # on a particular version of Bsdpkgs.
      evalBsdpkgs =
        let
          inherit (bsdpkgsFor.x86_64-linux.native) runCommand bsd;
        in
        runCommand "eval-bsdos" { buildInputs = [ bsd ]; } ''
          type -p bsd-env
          # Note: we're filtering out bsdos-install-tools because https://github.com/BasedLinux/bsdpkgs/pull/153594#issuecomment-1020530593.
          (
            set -x
            time bsd-env --store dummy:// -f ${bsdpkgs-regression} -qaP --drv-path | sort | grep -v bsdos-install-tools > packages
            [[ $(sha1sum < packages | cut -c1-40) = e01b031fc9785a572a38be6bc473957e3b6faad7 ]]
          )
          mkdir $out
        '';

      bsdpkgsLibTests = forAllSystems (
        system:
        import (bsdpkgs + "/lib/tests/test-with-bsd.bsd") {
          lib = bsdpkgsFor.${system}.native.lib;
          bsd = self.packages.${system}.bsd-cli;
          pkgs = bsdpkgsFor.${system}.native;
        }
      );
    };

  metrics.bsdpkgs = import "${bsdpkgs-regression}/pkgs/top-level/metrics.bsd" {
    pkgs = bsdpkgsFor.x86_64-linux.native;
    bsdpkgs = bsdpkgs-regression;
  };

  installTests = forAllSystems (
    system:
    let
      pkgs = bsdpkgsFor.${system}.native;
    in
    pkgs.runCommand "install-tests" {
      againstSelf = testBsdVersions pkgs pkgs.bsd;
      againstCurrentLatest =
        # FIXME: temporarily disable this on macOS because of #3605.
        if system == "x86_64-linux" then testBsdVersions pkgs pkgs.bsdVersions.latest else null;
      # Disabled because the latest stable version doesn't handle
      # `NIX_DAEMON_SOCKET_PATH` which is required for the tests to work
      # againstLatestStable = testBsdVersions pkgs pkgs.bsdStable;
    } "touch $out"
  );

  installerTests = import ../tests/installer {
    binaryTarballs = self.hydraJobs.binaryTarball;
    inherit bsdpkgsFor;
  };
}
