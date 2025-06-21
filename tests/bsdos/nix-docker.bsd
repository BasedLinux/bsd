# Test the container built by ../../docker.bsd.

{
  config,
  ...
}:

let
  pkgs = config.nodes.machine.bsdpkgs.pkgs;

  bsdImage = pkgs.callPackage ../../docker.bsd { };
  bsdUserImage = pkgs.callPackage ../../docker.bsd {
    name = "bsd-user";
    uid = 1000;
    gid = 1000;
    uname = "user";
    gname = "user";
  };

  containerTestScript = ./bsd-docker-test.sh;

in
{
  name = "bsd-docker";

  nodes = {
    machine =
      {
        config,
        lib,
        pkgs,
        ...
      }:
      {
        virtualisation.diskSize = 4096;
      };
    cache =
      {
        config,
        lib,
        pkgs,
        ...
      }:
      {
        virtualisation.additionalPaths = [
          pkgs.stdenv
          pkgs.hello
        ];
        services.harmonia.enable = true;
        networking.firewall.allowedTCPPorts = [ 5000 ];
      };
  };

  testScript =
    { nodes }:
    ''
      cache.wait_for_unit("harmonia.service")
      cache.wait_for_unit("network-addresses-eth1.service")

      machine.succeed("mkdir -p /etc/containers")
      machine.succeed("""echo '{"default":[{"type":"insecureAcceptAnything"}]}' > /etc/containers/policy.json""")

      machine.succeed("${pkgs.podman}/bin/podman load -i ${bsdImage}")
      machine.succeed("${pkgs.podman}/bin/podman run --rm bsd bsd --version")
      machine.succeed("${pkgs.podman}/bin/podman run --rm -i bsd < ${containerTestScript}")

      machine.succeed("${pkgs.podman}/bin/podman load -i ${bsdUserImage}")
      machine.succeed("${pkgs.podman}/bin/podman run --rm bsd-user bsd --version")
      machine.succeed("${pkgs.podman}/bin/podman run --rm -i bsd-user < ${containerTestScript}")
      machine.succeed("[[ $(${pkgs.podman}/bin/podman run --rm bsd-user stat -c %u /bsd/store) = 1000 ]]")
    '';
}
