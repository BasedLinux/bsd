{
  lib,
  config,
  hostPkgs,
  bsdpkgs,
  ...
}:

let
  pkgs = config.nodes.sourcehut.bsdpkgs.pkgs;

  # Generate a fake root CA and a fake git.sr.ht certificate.
  cert = pkgs.runCommand "cert" { buildInputs = [ pkgs.openssl ]; } ''
    mkdir -p $out

    openssl genrsa -out ca.key 2048
    openssl req -new -x509 -days 36500 -key ca.key \
      -subj "/C=NL/ST=Denial/L=Springfield/O=Dis/CN=Root CA" -out $out/ca.crt

    openssl req -newkey rsa:2048 -nodes -keyout $out/server.key \
      -subj "/C=CN/ST=Denial/L=Springfield/O=Dis/CN=git.sr.ht" -out server.csr
    openssl x509 -req -extfile <(printf "subjectAltName=DNS:git.sr.ht") \
      -days 36500 -in server.csr -CA $out/ca.crt -CAkey ca.key -CAcreateserial -out $out/server.crt
  '';

  registry = pkgs.writeTextFile {
    name = "registry";
    text = ''
      {
        "flakes": [
          {
            "from": {
              "type": "indirect",
              "id": "bsdpkgs"
            },
            "to": {
              "type": "sourcehut",
              "owner": "~BasedLinux",
              "repo": "bsdpkgs"
            }
          }
        ],
        "version": 2
      }
    '';
    destination = "/flake-registry.json";
  };

  bsdpkgs-repo = pkgs.runCommand "bsdpkgs-flake" { } ''
    dir=BasedLinux-bsdpkgs-${bsdpkgs.shortRev}
    cp -prd ${bsdpkgs} $dir

    # Set the correct timestamp in the tarball.
    find $dir -print0 | xargs -0 touch -h -t ${builtins.substring 0 12 bsdpkgs.lastModifiedDate}.${
      builtins.substring 12 2 bsdpkgs.lastModifiedDate
    } --

    mkdir -p $out/archive
    tar cfz $out/archive/${bsdpkgs.rev}.tar.gz $dir --hard-dereference

    echo 'ref: refs/heads/master' > $out/HEAD

    mkdir -p $out/info
    echo -e '${bsdpkgs.rev}\trefs/heads/master\n${bsdpkgs.rev}\trefs/tags/foo-bar' > $out/info/refs
  '';

in

{
  name = "sourcehut-flakes";

  nodes = {
    # Impersonate git.sr.ht
    sourcehut =
      { config, pkgs, ... }:
      {
        networking.firewall.allowedTCPPorts = [
          80
          443
        ];

        services.httpd.enable = true;
        services.httpd.adminAddr = "foo@example.org";
        services.httpd.extraConfig = ''
          ErrorLog syslog:local6
        '';
        services.httpd.virtualHosts."git.sr.ht" = {
          forceSSL = true;
          sslServerKey = "${cert}/server.key";
          sslServerCert = "${cert}/server.crt";
          servedDirs = [
            {
              urlPath = "/~BasedLinux/bsdpkgs";
              dir = bsdpkgs-repo;
            }
            {
              urlPath = "/~BasedLinux/flake-registry/blob/master";
              dir = registry;
            }
          ];
        };
      };

    client =
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
          pkgs.hello
          pkgs.fuse
        ];
        virtualisation.memorySize = 4096;
        bsd.settings.substituters = lib.mkForce [ ];
        bsd.extraOptions = ''
          experimental-features = bsd-command flakes
          flake-registry = https://git.sr.ht/~BasedLinux/flake-registry/blob/master/flake-registry.json
        '';
        environment.systemPackages = [ pkgs.jq ];
        networking.hosts.${(builtins.head nodes.sourcehut.networking.interfaces.eth1.ipv4.addresses).address} =
          [ "git.sr.ht" ];
        security.pki.certificateFiles = [ "${cert}/ca.crt" ];
      };
  };

  testScript =
    { nodes }:
    ''
      # fmt: off
      import json
      import time

      start_all()

      sourcehut.wait_for_unit("httpd.service")
      sourcehut.wait_for_unit("network-addresses-eth1.service")
      client.wait_for_unit("network-addresses-eth1.service")

      client.succeed("curl -v https://git.sr.ht/ >&2")
      client.succeed("bsd registry list | grep bsdpkgs")

      # Test that it resolves HEAD
      rev = client.succeed("bsd flake info sourcehut:~BasedLinux/bsdpkgs --json | jq -r .revision")
      assert rev.strip() == "${bsdpkgs.rev}", "revision mismatch"
      # Test that it resolves branches
      rev = client.succeed("bsd flake info sourcehut:~BasedLinux/bsdpkgs/master --json | jq -r .revision")
      assert rev.strip() == "${bsdpkgs.rev}", "revision mismatch"
      # Test that it resolves tags
      rev = client.succeed("bsd flake info sourcehut:~BasedLinux/bsdpkgs/foo-bar --json | jq -r .revision")
      assert rev.strip() == "${bsdpkgs.rev}", "revision mismatch"

      # Registry and pinning test
      rev = client.succeed("bsd flake info bsdpkgs --json | jq -r .revision")
      assert rev.strip() == "${bsdpkgs.rev}", "revision mismatch"

      client.succeed("bsd registry pin bsdpkgs")

      client.succeed("bsd flake info bsdpkgs --tarball-ttl 0 >&2")

      # Shut down the web server. The flake should be cached on the client.
      sourcehut.succeed("systemctl stop httpd.service")

      info = json.loads(client.succeed("bsd flake info bsdpkgs --json"))
      date = time.strftime("%Y%m%d%H%M%S", time.gmtime(info['lastModified']))
      assert date == "${bsdpkgs.lastModifiedDate}", "time mismatch"

      client.succeed("bsd build bsdpkgs#hello")

      # The build shouldn't fail even with --tarball-ttl 0 (the server
      # being down should not be a fatal error).
      client.succeed("bsd build bsdpkgs#fuse --tarball-ttl 0")
    '';

}
