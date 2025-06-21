with import ./config.nix;

builtins.outputOf
  (mkDerivation {
    name = "make-derivations.drv";

    requiredSystemFeatures = [ "recursive-bsd" ];

    buildCommand = ''
      set -e
      set -u

      PATH=${builtins.getEnv "NIX_BIN_DIR"}:$PATH

      export NIX_CONFIG='extra-experimental-features = bsd-command ca-derivations dynamic-derivations'

      declare -A deps=(
        [a]=""
        [b]="a"
        [c]="a"
        [d]="b c"
        [e]="b c d"
      )

      # Cannot just literally include this, or Bsd will think it is the
      # *outer* derivation that's trying to refer to itself, and
      # substitute the string too soon.
      placeholder=$(bsd eval --raw --expr 'builtins.placeholder "out"')

      declare -A drvs=()
      for word in a b c d e; do
        inputDrvs=""
        for dep in ''${deps[$word]}; do
          if [[ "$inputDrvs" != "" ]]; then
            inputDrvs+=","
          fi
          read -r -d "" line <<EOF || true
          "''${drvs[$dep]}": {
            "outputs": ["out"],
            "dynamicOutputs": {}
          }
      EOF
          inputDrvs+="$line"
        done
        read -r -d "" json <<EOF || true
        {
          "args": ["-c", "set -xeu; echo \"word env vav $word is \$$word\" >> \"\$out\""],
          "builder": "${shell}",
          "env": {
            "out": "$placeholder",
            "$word": "hello, from $word!",
            "PATH": ${builtins.toJSON path}
          },
          "inputDrvs": {
            $inputDrvs
          },
          "inputSrcs": [],
          "name": "build-$word",
          "outputs": {
            "out": {
              "method": "nar",
              "hashAlgo": "sha256"
            }
          },
          "system": "${system}"
        }
      EOF
        drvs[$word]="$(echo "$json" | bsd derivation add)"
      done
      cp "''${drvs[e]}" $out
    '';

    __contentAddressed = true;
    outputHashMode = "text";
    outputHashAlgo = "sha256";
  }).outPath
  "out"
