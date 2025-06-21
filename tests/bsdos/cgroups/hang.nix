{ }:

with import <bsdpkgs> { };

runCommand "hang"
  {
    requiredSystemFeatures = "uid-range";
  }
  ''
    sleep infinity
  ''
