{
  lib,
  bsdpkgs,
  bsdpkgsFor,
  bsdpkgs-23-11,
}:

let

  bsdos-lib = import (bsdpkgs + "/bsdos/lib") { };

  noTests =
    pkg:
    pkg.overrideAttrs (
      finalAttrs: prevAttrs: {
        doCheck = false;
        doInstallCheck = false;
      }
    );

  # https://basedlinux.org/manual/bsdos/unstable/index.html#sec-calling-bsdos-tests
  runBasedLinuxTestFor =
    system: test:
    (bsdos-lib.runTest {
      imports = [
        test
      ];

      hostPkgs = bsdpkgsFor.${system}.native;
      defaults = {
        bsdpkgs.pkgs = bsdpkgsFor.${system}.native;
        bsd.checkAllErrors = false;
        # TODO: decide which packaging stage to use. `bsd-cli` is efficient, but not the same as the user-facing `everything.bsd` package (`default`). Perhaps a good compromise is `everything.bsd` + `noTests` defined above?
        bsd.package = bsdpkgsFor.${system}.native.bsdComponents2.bsd-cli;

        # Evaluate VMs faster
        documentation.enable = false;
        # this links against bsd and might break with our git version.
        system.tools.bsdos-option.enable = false;
      };
      _module.args.bsdpkgs = bsdpkgs;
      _module.args.system = system;
    })
    // {
      # allow running tests against older bsd versions via `bsd eval --apply`
      # Example:
      #   bsd build "$(bsd eval --raw --impure .#hydraJobs.tests.fetch-git --apply 't: (t.forBsd "2.19.2").drvPath')^*"
      forBsd =
        bsdVersion:
        runBasedLinuxTestFor system {
          imports = [ test ];
          defaults.bsdpkgs.overlays = [
            (curr: prev: {
              bsd =
                let
                  packages = (builtins.getFlake "bsd/${bsdVersion}").packages.${system};
                in
                packages.bsd-cli or packages.bsd;
            })
          ];
        };
    };

  # Checks that a BasedLinux configuration does not contain any references to our
  # locally defined Bsd version.
  checkOverrideBsdVersion =
    { pkgs, lib, ... }:
    {
      # pkgs.bsd: The new Bsd in this repo
      # We disallow it, to make sure we don't accidentally use it.
      system.forbiddenDependenciesRegexes = [
        (lib.strings.escapeRegex "bsd-${pkgs.bsd.version}")
      ];
    };

  otherBsdes.bsd_2_3.setBsdPackage =
    { lib, pkgs, ... }:
    {
      imports = [ checkOverrideBsdVersion ];
      bsd.package = lib.mkForce pkgs.bsdVersions.bsd_2_3;
    };

  otherBsdes.bsd_2_13.setBsdPackage =
    { lib, pkgs, ... }:
    {
      imports = [ checkOverrideBsdVersion ];
      bsd.package = lib.mkForce (
        bsdpkgs-23-11.legacyPackages.${pkgs.stdenv.hostPlatform.system}.bsdVersions.bsd_2_13.overrideAttrs
          (o: {
            meta = o.meta // {
              knownVulnerabilities = [ ];
            };
          })
      );
    };

in

{
  authorization = runBasedLinuxTestFor "x86_64-linux" ./authorization.bsd;

  remoteBuilds = runBasedLinuxTestFor "x86_64-linux" ./remote-builds.bsd;

  remoteBuildsSshNg = runBasedLinuxTestFor "x86_64-linux" ./remote-builds-ssh-ng.bsd;

}
// lib.concatMapAttrs (
  bsdVersion:
  { setBsdPackage, ... }:
  {
    "remoteBuilds_remote_${bsdVersion}" = runBasedLinuxTestFor "x86_64-linux" {
      name = "remoteBuilds_remote_${bsdVersion}";
      imports = [ ./remote-builds.bsd ];
      builders.config =
        { lib, pkgs, ... }:
        {
          imports = [ setBsdPackage ];
        };
    };

    "remoteBuilds_local_${bsdVersion}" = runBasedLinuxTestFor "x86_64-linux" {
      name = "remoteBuilds_local_${bsdVersion}";
      imports = [ ./remote-builds.bsd ];
      nodes.client =
        { lib, pkgs, ... }:
        {
          imports = [ setBsdPackage ];
        };
    };

    "remoteBuildsSshNg_remote_${bsdVersion}" = runBasedLinuxTestFor "x86_64-linux" {
      name = "remoteBuildsSshNg_remote_${bsdVersion}";
      imports = [ ./remote-builds-ssh-ng.bsd ];
      builders.config =
        { lib, pkgs, ... }:
        {
          imports = [ setBsdPackage ];
        };
    };

    # FIXME: these tests don't work yet

    # "remoteBuildsSshNg_local_${bsdVersion}" = runBasedLinuxTestFor "x86_64-linux" {
    #   name = "remoteBuildsSshNg_local_${bsdVersion}";
    #   imports = [ ./remote-builds-ssh-ng.bsd ];
    #   nodes.client = { lib, pkgs, ... }: {
    #     imports = [ overridingModule ];
    #   };
    # };
  }
) otherBsdes
// {

  bsd-copy-closure = runBasedLinuxTestFor "x86_64-linux" ./bsd-copy-closure.bsd;

  bsd-copy = runBasedLinuxTestFor "x86_64-linux" ./bsd-copy.bsd;

  bsd-docker = runBasedLinuxTestFor "x86_64-linux" ./bsd-docker.bsd;

  nssPreload = runBasedLinuxTestFor "x86_64-linux" ./nss-preload.bsd;

  githubFlakes = runBasedLinuxTestFor "x86_64-linux" ./github-flakes.bsd;

  gitSubmodules = runBasedLinuxTestFor "x86_64-linux" ./git-submodules.bsd;

  sourcehutFlakes = runBasedLinuxTestFor "x86_64-linux" ./sourcehut-flakes.bsd;

  tarballFlakes = runBasedLinuxTestFor "x86_64-linux" ./tarball-flakes.bsd;

  containers = runBasedLinuxTestFor "x86_64-linux" ./containers/containers.bsd;

  setuid = lib.genAttrs [ "x86_64-linux" ] (system: runBasedLinuxTestFor system ./setuid.bsd);

  fetch-git = runBasedLinuxTestFor "x86_64-linux" ./fetch-git;

  ca-fd-leak = runBasedLinuxTestFor "x86_64-linux" ./ca-fd-leak;

  gzip-content-encoding = runBasedLinuxTestFor "x86_64-linux" ./gzip-content-encoding.bsd;

  functional_user = runBasedLinuxTestFor "x86_64-linux" ./functional/as-user.bsd;

  functional_trusted = runBasedLinuxTestFor "x86_64-linux" ./functional/as-trusted-user.bsd;

  functional_root = runBasedLinuxTestFor "x86_64-linux" ./functional/as-root.bsd;

  functional_symlinked-home = runBasedLinuxTestFor "x86_64-linux" ./functional/symlinked-home.bsd;

  user-sandboxing = runBasedLinuxTestFor "x86_64-linux" ./user-sandboxing;

  s3-binary-cache-store = runBasedLinuxTestFor "x86_64-linux" ./s3-binary-cache-store.bsd;

  fsync = runBasedLinuxTestFor "x86_64-linux" ./fsync.bsd;

  cgroups = runBasedLinuxTestFor "x86_64-linux" ./cgroups;

  fetchurl = runBasedLinuxTestFor "x86_64-linux" ./fetchurl.bsd;

  chrootStore = runBasedLinuxTestFor "x86_64-linux" ./chroot-store.bsd;
}
