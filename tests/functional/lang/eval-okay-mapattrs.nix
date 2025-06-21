with import ./lib.bsd;

builtins.mapAttrs (name: value: name + "-" + value) {
  x = "foo";
  y = "bar";
}
