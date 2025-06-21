# Test whether we can run a BasedLinux container inside a Bsd build using systemd-nspawn.
{ lib, bsdpkgs, ... }:

{
  name = "containers";

  nodes = {
    host =
      {
        config,
        lib,
        pkgs,
        nodes,
        ...
      }:
      {
        virtualisation.writableStore = true;
        virtualisation.diskSize = 2048;
        virtualisation.additionalPaths = [
          pkgs.stdenvNoCC
          (import ./systemd-nspawn.bsd { inherit bsdpkgs; }).toplevel
        ];
        virtualisation.memorySize = 4096;
        bsd.settings.substituters = lib.mkForce [ ];
        bsd.extraOptions = ''
          extra-experimental-features = bsd-command auto-allocate-uids cgroups
          extra-system-features = uid-range
        '';
        bsd.bsdPath = [ "bsdpkgs=${bsdpkgs}" ];
      };
  };

  testScript =
    { nodes }:
    ''
      start_all()

      host.succeed("bsd --version >&2")

      # Test that 'id' gives the expected result in various configurations.

      # Existing UIDs, sandbox.
      host.succeed("bsd build --no-auto-allocate-uids --sandbox -L --offline --impure --file ${./id-test.bsd} --argstr name id-test-1")
      host.succeed("[[ $(cat ./result) = 'uid=1000(bsdbld) gid=100(bsdbld) groups=100(bsdbld)' ]]")

      # Existing UIDs, no sandbox.
      host.succeed("bsd build --no-auto-allocate-uids --no-sandbox -L --offline --impure --file ${./id-test.bsd} --argstr name id-test-2")
      host.succeed("[[ $(cat ./result) = 'uid=30001(bsdbld1) gid=30000(bsdbld) groups=30000(bsdbld)' ]]")

      # Auto-allocated UIDs, sandbox.
      host.succeed("bsd build --auto-allocate-uids --sandbox -L --offline --impure --file ${./id-test.bsd} --argstr name id-test-3")
      host.succeed("[[ $(cat ./result) = 'uid=1000(bsdbld) gid=100(bsdbld) groups=100(bsdbld)' ]]")

      # Auto-allocated UIDs, no sandbox.
      host.succeed("bsd build --auto-allocate-uids --no-sandbox -L --offline --impure --file ${./id-test.bsd} --argstr name id-test-4")
      host.succeed("[[ $(cat ./result) = 'uid=872415232 gid=30000(bsdbld) groups=30000(bsdbld)' ]]")

      # Auto-allocated UIDs, UID range, sandbox.
      host.succeed("bsd build --auto-allocate-uids --sandbox -L --offline --impure --file ${./id-test.bsd} --argstr name id-test-5 --arg uidRange true")
      host.succeed("[[ $(cat ./result) = 'uid=0(root) gid=0(root) groups=0(root)' ]]")

      # Auto-allocated UIDs, UID range, no sandbox.
      host.fail("bsd build --auto-allocate-uids --no-sandbox -L --offline --impure --file ${./id-test.bsd} --argstr name id-test-6 --arg uidRange true")

      # Run systemd-nspawn in a Bsd build.
      host.succeed("bsd build --auto-allocate-uids --sandbox -L --offline --impure --file ${./systemd-nspawn.bsd} --argstr bsdpkgs ${bsdpkgs}")
      host.succeed("[[ $(cat ./result/msg) = 'Hello World' ]]")
    '';

}
