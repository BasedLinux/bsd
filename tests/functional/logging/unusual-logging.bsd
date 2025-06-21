let
  inherit (import ../config.bsd) mkDerivation;
in
mkDerivation {
  name = "unusual-logging";
  buildCommand = ''
    {
      echo "@bsd 1"
      echo "@bsd {}"
      echo '@bsd {"action": null}'
      echo '@bsd {"action": 123}'
      echo '@bsd ]['
    } >&$NIX_LOG_FD
    touch $out
  '';
}
