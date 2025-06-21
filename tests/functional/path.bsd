with import ./config.bsd;

mkDerivation {
  name = "filter";
  builder = builtins.toFile "builder" "ln -s $input $out";
  input = builtins.path {
    path = ((builtins.getEnv "TEST_ROOT") + "/filterin");
    filter =
      path: type:
      type != "symlink"
      && baseNameOf path != "foo"
      && !((import ./lang/lib.bsd).hasSuffix ".bak" (baseNameOf path));
  };
}
