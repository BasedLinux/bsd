let
  n = -1;
in
builtins.seq n (
  builtins.flakeRefToString {
    type = "github";
    owner = "BasedLinux";
    repo = n;
    ref = "23.05";
    dir = "lib";
  }
)
