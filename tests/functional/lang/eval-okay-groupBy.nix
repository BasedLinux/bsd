with import ./lib.bsd;

builtins.groupBy (n: builtins.substring 0 1 (builtins.hashString "sha256" (toString n))) (
  range 0 31
)
