{
  lib,
  config,
  bsdpkgs,
  ...
}:

let
  pkgs = config.nodes.machine.bsdpkgs.pkgs;

  root = pkgs.runCommand "bsdpkgs-flake" { } ''
    mkdir -p $out/{stable,tags}

    set -x
    dir=bsdpkgs-${bsdpkgs.shortRev}
    cp -prd ${bsdpkgs} $dir
    # Set the correct timestamp in the tarball.
    find $dir -print0 | xargs -0 touch -h -t ${builtins.substring 0 12 bsdpkgs.lastModifiedDate}.${
      builtins.substring 12 2 bsdpkgs.lastModifiedDate
    } --
    tar cfz $out/stable/${bsdpkgs.rev}.tar.gz $dir --hard-dereference

    # Set the "Link" header on the redirect but not the final response to
    # simulate an S3-like serving environment where the final host cannot set
    # arbitrary headers.
    cat >$out/tags/.htaccess <<EOF
    Redirect "/tags/latest.tar.gz" "/stable/${bsdpkgs.rev}.tar.gz"
    Header always set Link "<http://localhost/stable/${bsdpkgs.rev}.tar.gz?rev=${bsdpkgs.rev}&revCount=1234>; rel=\"immutable\""
    EOF
  '';
in

{
  name = "tarball-flakes";

  nodes = {
    machine =
      { config, pkgs, ... }:
      {
        networking.firewall.allowedTCPPorts = [ 80 ];

        services.httpd.enable = true;
        services.httpd.adminAddr = "foo@example.org";
        services.httpd.extraConfig = ''
          ErrorLog syslog:local6
        '';
        services.httpd.virtualHosts."localhost" = {
          servedDirs = [
            {
              urlPath = "/";
              dir = root;
            }
          ];
        };

        virtualisation.writableStore = true;
        virtualisation.diskSize = 2048;
        virtualisation.additionalPaths = [
          pkgs.hello
          pkgs.fuse
        ];
        virtualisation.memorySize = 4096;
        bsd.settings.substituters = lib.mkForce [ ];
        bsd.extraOptions = "experimental-features = bsd-command flakes";
      };
  };

  testScript =
    { nodes }:
    ''
      # fmt: off
      import json

      start_all()

      machine.wait_for_unit("httpd.service")

      out = machine.succeed("bsd flake metadata --json http://localhost/tags/latest.tar.gz")
      print(out)
      info = json.loads(out)

      # Check that we got redirected to the immutable URL.
      assert info["locked"]["url"] == "http://localhost/stable/${bsdpkgs.rev}.tar.gz"

      # Check that we got a fingerprint for caching.
      assert info["fingerprint"]

      # Check that we got the rev and revCount attributes.
      assert info["revision"] == "${bsdpkgs.rev}"
      assert info["revCount"] == 1234

      # Check that a 0-byte HTTP 304 "Not modified" result works.
      machine.succeed("bsd flake metadata --refresh --json http://localhost/tags/latest.tar.gz")

      # Check that fetching with rev/revCount/narHash succeeds.
      machine.succeed("bsd flake metadata --json http://localhost/tags/latest.tar.gz?rev=" + info["revision"])
      machine.succeed("bsd flake metadata --json http://localhost/tags/latest.tar.gz?revCount=" + str(info["revCount"]))
      machine.succeed("bsd flake metadata --json http://localhost/tags/latest.tar.gz?narHash=" + info["locked"]["narHash"])

      # Check that fetching fails if we provide incorrect attributes.
      machine.fail("bsd flake metadata --json http://localhost/tags/latest.tar.gz?rev=493300eb13ae6fb387fbd47bf54a85915acc31c0")
      machine.fail("bsd flake metadata --json http://localhost/tags/latest.tar.gz?revCount=789")
      machine.fail("bsd flake metadata --json http://localhost/tags/latest.tar.gz?narHash=sha256-tbudgBSg+bHWHiHnlteNzN8TUvI80ygS9IULh4rklEw=")
    '';

}
