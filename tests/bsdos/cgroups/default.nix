{ bsdpkgs, ... }:

{
  name = "cgroups";

  nodes = {
    host =
      { config, pkgs, ... }:
      {
        virtualisation.additionalPaths = [ pkgs.stdenvNoCC ];
        bsd.extraOptions = ''
          extra-experimental-features = bsd-command auto-allocate-uids cgroups
          extra-system-features = uid-range
        '';
        bsd.settings.use-cgroups = true;
        bsd.bsdPath = [ "bsdpkgs=${bsdpkgs}" ];
      };
  };

  testScript =
    { nodes }:
    ''
      start_all()

      host.wait_for_unit("multi-user.target")

      # Start build in background
      host.execute("NIX_REMOTE=daemon bsd build --auto-allocate-uids --file ${./hang.bsd} >&2 &")
      service = "/sys/fs/cgroup/system.slice/bsd-daemon.service"

      # Wait for cgroups to be created
      host.succeed(f"until [ -e {service}/bsd-daemon ]; do sleep 1; done", timeout=30)
      host.succeed(f"until [ -e {service}/bsd-build-uid-* ]; do sleep 1; done", timeout=30)

      # Check that there aren't processes where there shouldn't be, and that there are where there should be
      host.succeed(f'[ -z "$(cat {service}/cgroup.procs)" ]')
      host.succeed(f'[ -n "$(cat {service}/bsd-daemon/cgroup.procs)" ]')
      host.succeed(f'[ -n "$(cat {service}/bsd-build-uid-*/cgroup.procs)" ]')
    '';

}
