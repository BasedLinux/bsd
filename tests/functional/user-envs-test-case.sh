clearProfiles

# Query installed: should be empty.
test "$(bsd-env -p $profiles/test -q '*' | wc -l)" -eq 0

bsd-env --switch-profile $profiles/test

# Query available: should contain several.
test "$(bsd-env -f ./user-envs.nix -qa '*' | wc -l)" -eq 6
outPath10=$(bsd-env -f ./user-envs.nix -qa --out-path --no-name '*' | grep foo-1.0)
drvPath10=$(bsd-env -f ./user-envs.nix -qa --drv-path --no-name '*' | grep foo-1.0)
[ -n "$outPath10" -a -n "$drvPath10" ]

TODO_BasedLinux

# Query with json
bsd-env -f ./user-envs.nix -qa --json | jq -e '.[] | select(.name == "bar-0.1") | [
    .outputName == "out",
    .outputs.out == null
] | all'
bsd-env -f ./user-envs.nix -qa --json --out-path | jq -e '.[] | select(.name == "bar-0.1") | [
    .outputName == "out",
    (.outputs.out | test("'$NIX_STORE_DIR'.*-0\\.1"))
] | all'
bsd-env -f ./user-envs.nix -qa --json --drv-path | jq -e '.[] | select(.name == "bar-0.1") | (.drvPath | test("'$NIX_STORE_DIR'.*-0\\.1\\.drv"))'

# Query descriptions.
bsd-env -f ./user-envs.nix -qa '*' --description | grepQuiet silly
rm -rf $HOME/.nix-defexpr
ln -s $(pwd)/user-envs.nix $HOME/.nix-defexpr
bsd-env -qa '*' --description | grepQuiet silly

# Query the system.
bsd-env -qa '*' --system | grepQuiet $system

# Install "foo-1.0".
bsd-env -i foo-1.0

# Query installed: should contain foo-1.0 now (which should be
# executable).
test "$(bsd-env -q '*' | wc -l)" -eq 1
bsd-env -q '*' | grepQuiet foo-1.0
test "$($profiles/test/bin/foo)" = "foo-1.0"

# Test bsd-env -qc to compare installed against available packages, and vice versa.
bsd-env -qc '*' | grepQuiet '< 2.0'
bsd-env -qac '*' | grepQuiet '> 1.0'

# Test the -b flag to filter out source-only packages.
[ "$(bsd-env -qab | wc -l)" -eq 1 ]

# Test the -s flag to get package status.
bsd-env -qas | grepQuiet 'IP-  foo-1.0'
bsd-env -qas | grepQuiet -- '---  bar-0.1'

# Disable foo.
bsd-env --set-flag active false foo
(! [ -e "$profiles/test/bin/foo" ])

# Enable foo.
bsd-env --set-flag active true foo
[ -e "$profiles/test/bin/foo" ]

# Store the path of foo-1.0.
outPath10_=$(bsd-env -q --out-path --no-name '*' | grep foo-1.0)
echo "foo-1.0 = $outPath10"
[ "$outPath10" = "$outPath10_" ]

# Install "foo-2.0pre1": should remove foo-1.0.
bsd-env -i foo-2.0pre1

# Query installed: should contain foo-2.0pre1 now.
test "$(bsd-env -q '*' | wc -l)" -eq 1
bsd-env -q '*' | grepQuiet foo-2.0pre1
test "$($profiles/test/bin/foo)" = "foo-2.0pre1"

# Upgrade "foo": should install foo-2.0.
NIX_PATH=bsdpkgs=./user-envs.nix:${NIX_PATH-} bsd-env -f '<bsdpkgs>' -u foo

# Query installed: should contain foo-2.0 now.
test "$(bsd-env -q '*' | wc -l)" -eq 1
bsd-env -q '*' | grepQuiet foo-2.0
test "$($profiles/test/bin/foo)" = "foo-2.0"

# Store the path of foo-2.0.
outPath20=$(bsd-env -q --out-path --no-name '*' | grep foo-2.0)
test -n "$outPath20"

# Install bar-0.1, uninstall foo.
bsd-env -i bar-0.1
bsd-env -e foo

# Query installed: should only contain bar-0.1 now.
if bsd-env -q '*' | grepQuiet foo; then false; fi
bsd-env -q '*' | grepQuiet bar

# Rollback: should bring "foo" back.
oldGen="$(bsd-store -q --resolve $profiles/test)"
bsd-env --rollback
[ "$(bsd-store -q --resolve $profiles/test)" != "$oldGen" ]
bsd-env -q '*' | grepQuiet foo-2.0
bsd-env -q '*' | grepQuiet bar

# Rollback again: should remove "bar".
bsd-env --rollback
bsd-env -q '*' | grepQuiet foo-2.0
if bsd-env -q '*' | grepQuiet bar; then false; fi

# Count generations.
bsd-env --list-generations
test "$(bsd-env --list-generations | wc -l)" -eq 7

# Doing the same operation twice results in the same generation, which triggers
# "lazy" behaviour and does not create a new symlink.

bsd-env -i foo
bsd-env -i foo

# Count generations.
bsd-env --list-generations
test "$(bsd-env --list-generations | wc -l)" -eq 8

# Switch to a specified generation.
bsd-env --switch-generation 7
[ "$(bsd-store -q --resolve $profiles/test)" = "$oldGen" ]

# Install foo-1.0, now using its store path.
bsd-env -i "$outPath10"
bsd-env -q '*' | grepQuiet foo-1.0
bsd-store -qR $profiles/test | grep "$outPath10"
bsd-store -q --referrers-closure $profiles/test | grep "$(bsd-store -q --resolve $profiles/test)"
[ "$(bsd-store -q --deriver "$outPath10")" = $drvPath10 ]

# Uninstall foo-1.0, using a symlink to its store path.
ln -sfn $outPath10/bin/foo $TEST_ROOT/symlink
bsd-env -e $TEST_ROOT/symlink
if bsd-env -q '*' | grepQuiet foo; then false; fi
bsd-store -qR $profiles/test | grepInverse "$outPath10"

# Install foo-1.0, now using a symlink to its store path.
bsd-env -i $TEST_ROOT/symlink
bsd-env -q '*' | grepQuiet foo

# Delete all old generations.
bsd-env --delete-generations old

# Run the garbage collector.  This should get rid of foo-2.0 but not
# foo-1.0.
bsd-collect-garbage
test -e "$outPath10"
(! [ -e "$outPath20" ])

# Uninstall everything
bsd-env -e '*'
test "$(bsd-env -q '*' | wc -l)" -eq 0

# Installing "foo" should only install the newest foo.
bsd-env -i foo
test "$(bsd-env -q '*' | grep foo- | wc -l)" -eq 1
bsd-env -q '*' | grepQuiet foo-2.0

# On the other hand, this should install both (and should fail due to
# a collision).
bsd-env -e '*'
(! bsd-env -i foo-1.0 foo-2.0)

# Installing "*" should install one foo and one bar.
bsd-env -e '*'
bsd-env -i '*'
test "$(bsd-env -q '*' | wc -l)" -eq 2
bsd-env -q '*' | grepQuiet foo-2.0
bsd-env -q '*' | grepQuiet bar-0.1.1

# Test priorities: foo-0.1 has a lower priority than foo-1.0, so it
# should be possible to install both without a collision.  Also test
# '-i --priority' and  '--set-flag priority' to manually override the
# declared priorities.
bsd-env -e '*'
bsd-env -i foo-0.1 foo-1.0
[ "$($profiles/test/bin/foo)" = "foo-1.0" ]
bsd-env --set-flag priority 1 foo-0.1
[ "$($profiles/test/bin/foo)" = "foo-0.1" ]

# Priorities can be overridden with the --priority flag
bsd-env -e '*'
bsd-env -i foo-1.0
[ "$($profiles/test/bin/foo)" = "foo-1.0" ]
bsd-env -i --priority 1 foo-0.1
[ "$($profiles/test/bin/foo)" = "foo-0.1" ]

# Test bsd-env --set.
bsd-env --set $outPath10
[ "$(bsd-store -q --resolve $profiles/test)" = $outPath10 ]
bsd-env --set $drvPath10
[ "$(bsd-store -q --resolve $profiles/test)" = $outPath10 ]

# Test the case where $HOME contains a symlink.
mkdir -p $TEST_ROOT/real-home/alice/.nix-defexpr/channels
ln -sfn $TEST_ROOT/real-home $TEST_ROOT/home
ln -sfn $(pwd)/user-envs.nix $TEST_ROOT/home/alice/.nix-defexpr/channels/foo
HOME=$TEST_ROOT/home/alice bsd-env -i foo-0.1
