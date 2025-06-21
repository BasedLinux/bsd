{
  name,
  uidRange ? false,
}:

with import <bsdpkgs> { };

runCommand name {
  requiredSystemFeatures = if uidRange then [ "uid-range" ] else [ ];
} "id; id > $out"
