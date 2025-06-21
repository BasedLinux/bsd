with import ./config.bsd;
import (mkDerivation {
  name = "foo";
  bla = import ./dependencies.bsd { };
  buildCommand = "
      echo \\\"hi\\\" > $out
    ";
})
