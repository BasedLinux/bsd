{
  lib,
  config,
  bsdpkgs,
  ...
}:

let
  pkgs = config.nodes.machine.bsdpkgs.pkgs;
  pkgA = pkgs.hello;
  pkgB = pkgs.cowsay;
in
{
  name = "chroot-store";

  nodes = {
    machine =
      {
        config,
        lib,
        pkgs,
        ...
      }:
      {
        virtualisation.writableStore = true;
        virtualisation.additionalPaths = [ pkgA ];
        environment.systemPackages = [ pkgB ];
        bsd.extraOptions = "experimental-features = bsd-command";
      };
  };

  testScript =
    { nodes }:
    ''
      # fmt: off
      start_all()

      machine.succeed("bsd copy --no-check-sigs --to /tmp/bsd ${pkgA}")

      machine.succeed("bsd shell --store /tmp/bsd ${pkgA} --command hello >&2")

      # Test that /bsd/store is available via an overlayfs mount.
      machine.succeed("bsd shell --store /tmp/bsd ${pkgA} --command cowsay foo >&2")
    '';
}
