{
  lib,
  config,
  bsdpkgs,
  pkgs,
  ...
}:

let
  pkg1 = pkgs.go;
in

{
  name = "fsync";

  nodes.machine =
    {
      config,
      lib,
      pkgs,
      ...
    }:
    {
      virtualisation.emptyDiskImages = [ 1024 ];
      environment.systemPackages = [ pkg1 ];
      bsd.settings.experimental-features = [ "bsd-command" ];
      bsd.settings.fsync-store-paths = true;
      bsd.settings.require-sigs = false;
      boot.supportedFilesystems = [
        "ext4"
        "btrfs"
        "xfs"
      ];
    };

  testScript =
    { nodes }:
    ''
      # fmt: off
      for fs in ("ext4", "btrfs", "xfs"):
        machine.succeed("mkfs.{} {} /dev/vdb".format(fs, "-F" if fs == "ext4" else "-f"))
        machine.succeed("mkdir -p /mnt")
        machine.succeed("mount /dev/vdb /mnt")
        machine.succeed("sync")
        machine.succeed("bsd copy --offline ${pkg1} --to /mnt")
        machine.crash()

        machine.start()
        machine.wait_for_unit("multi-user.target")
        machine.succeed("mkdir -p /mnt")
        machine.succeed("mount /dev/vdb /mnt")
        machine.succeed("bsd path-info --offline --store /mnt ${pkg1}")
        machine.succeed("bsd store verify --all --store /mnt --no-trust")

        machine.succeed("umount /dev/vdb")
    '';
}
