with import ./config.bsd;

mkDerivation {
  name = "simple-failing";
  builder = builtins.toFile "builder.sh" ''
    echo "This should fail"
    exit 1
  '';
  PATH = "";
  goodPath = path;
}
