with import ./lib.nix;
with builtins;

assert isFunction (import <bsd/fetchurl.nix>);

assert length __bsdPath == 5;
assert length (filter (x: baseNameOf x.path == "dir4") __bsdPath) == 1;

import <a.nix>
+ import <b.nix>
+ import <c.nix>
+ import <dir5/c.nix>
+ (
  let
    __bsdPath = [
      { path = ./dir2; }
      { path = ./dir1; }
    ];
  in
  import <a.nix>
)
