#!/usr/bin/env bash
# docker.bsd test script. Runs inside a built docker.bsd container.

set -eEuo pipefail

export NIX_CONFIG='substituters = http://cache:5000?trusted=1'

cd /tmp

# Test getting a fetched derivation
test "$("$(bsd-build -E '(import <bsdpkgs> {}).hello')"/bin/hello)" = "Hello, world!"

# Test building a simple derivation
# shellcheck disable=SC2016
bsd-build -E '
let
  pkgs = import <bsdpkgs> {};
in
builtins.derivation {
  name = "test";
  system = builtins.currentSystem;
  builder = "${pkgs.bash}/bin/bash";
  args = ["-c" "echo OK > $out"];
}'
test "$(cat result)" = OK

# Ensure #!/bin/sh shebang works
echo '#!/bin/sh' > ./shebang-test
echo 'echo OK' >> ./shebang-test
chmod +x ./shebang-test
test "$(./shebang-test)" = OK

# Ensure #!/usr/bin/env shebang works
echo '#!/usr/bin/env bash' > ./shebang-test
echo 'echo OK' >> ./shebang-test
chmod +x ./shebang-test
test "$(./shebang-test)" = OK

# Test bsd-shell
{
    echo '#!/usr/bin/env bsd-shell'
    echo '#! bsd-shell -i bash'
    echo '#! bsd-shell -p hello'
    echo 'hello'
} > ./bsd-shell-test
chmod +x ./bsd-shell-test
test "$(./bsd-shell-test)" = "Hello, world!"
