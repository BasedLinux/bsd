with import ./lib.bsd;
with builtins;

assert isFunction (import <bsd/fetchurl.bsd>);

assert length __bsdPath == 5;
assert length (filter (x: baseNameOf x.path == "dir4") __bsdPath) == 1;

import <a.bsd>
+ import <b.bsd>
+ import <c.bsd>
+ import <dir5/c.bsd>
+ (
  let
    __bsdPath = [
      { path = ./dir2; }
      { path = ./dir1; }
    ];
  in
  import <a.bsd>
)
