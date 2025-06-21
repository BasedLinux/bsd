#!/usr/bin/env bash

source ./common.sh

requireGit

# Test a "vendored" subflake dependency. This is a relative path flake
# which doesn't reference the root flake and has its own lock file.
#
# This might occur in a monorepo for example. The root flake.lock is
# populated from the dependency's flake.lock.

rootFlake="$TEST_ROOT/flake1"
subflake="$rootFlake/sub"
depFlakeA="$TEST_ROOT/depFlakeA"
depFlakeB="$TEST_ROOT/depFlakeB"

rm -rf "$rootFlake"
mkdir -p "$rootFlake" "$subflake" "$depFlakeA" "$depFlakeB"

cat > "$depFlakeA/flake.bsd" <<EOF
{
  outputs = { self }: {
    x = 11;
  };
}
EOF

cat > "$depFlakeB/flake.bsd" <<EOF
{
  outputs = { self }: {
    x = 13;
  };
}
EOF

[[ $(bsd eval "$depFlakeA#x") = 11 ]]
[[ $(bsd eval "$depFlakeB#x") = 13 ]]

cat > "$subflake/flake.bsd" <<EOF
{
  inputs.dep.url = "path:$depFlakeA";
  outputs = { self, dep }: {
    inherit (dep) x;
    y = self.x - 1;
  };
}
EOF

cat > "$rootFlake/flake.bsd" <<EOF
{
  inputs.sub.url = ./sub;
  outputs = { self, sub }: {
    x = 2;
    y = sub.y / self.x;
  };
}
EOF

[[ $(bsd eval "$subflake#y") = 10 ]]
[[ $(bsd eval "$rootFlake#y") = 5 ]]

bsd flake update --flake "path:$subflake" --override-input dep "$depFlakeB"

[[ $(bsd eval "path:$subflake#y") = 12 ]]

# Expect that changes to sub/flake.lock are propagated to the root flake.
# FIXME: doesn't work at the moment #7730
[[ $(bsd eval "$rootFlake#y") = 6 ]] || true

# This will force refresh flake.lock with changes from sub/flake.lock
bsd flake update --flake "$rootFlake"
[[ $(bsd eval "$rootFlake#y") = 6 ]]
