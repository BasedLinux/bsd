#!/usr/bin/env bash

source common.sh

clearStoreIfPossible

if [[ -n ${NIX_TESTS_CA_BY_DEFAULT:-} ]]; then
    shellDotBsd="$PWD/ca-shell.bsd"
else
    shellDotBsd="$PWD/shell.bsd"
fi

export NIX_PATH=bsdpkgs="$shellDotBsd"

# Test bsd-shell -A
export IMPURE_VAR=foo
export SELECTED_IMPURE_VAR=baz

output=$(bsd-shell --pure "$shellDotBsd" -A shellDrv --run \
    'echo "$IMPURE_VAR - $VAR_FROM_STDENV_SETUP - $VAR_FROM_NIX - $TEST_inBsdShell"')

[ "$output" = " - foo - bar - true" ]

output=$(bsd-shell --pure "$shellDotBsd" -A shellDrv --option bsd-shell-always-looks-for-shell-bsd false --run \
    'echo "$IMPURE_VAR - $VAR_FROM_STDENV_SETUP - $VAR_FROM_NIX - $TEST_inBsdShell"')
[ "$output" = " - foo - bar - true" ]

# Test --keep
output=$(bsd-shell --pure --keep SELECTED_IMPURE_VAR "$shellDotBsd" -A shellDrv --run \
    'echo "$IMPURE_VAR - $VAR_FROM_STDENV_SETUP - $VAR_FROM_NIX - $SELECTED_IMPURE_VAR"')

[ "$output" = " - foo - bar - baz" ]

# test NIX_BUILD_TOP
testTmpDir=$(pwd)/bsd-shell
mkdir -p "$testTmpDir"
output=$(TMPDIR="$testTmpDir" bsd-shell --pure "$shellDotBsd" -A shellDrv --run 'echo $NIX_BUILD_TOP')
[[ "$output" =~ ${testTmpDir}.* ]] || {
    echo "expected $output =~ ${testTmpDir}.*" >&2
    exit 1
}

# Test bsd-shell on a .drv
[[ $(bsd-shell --pure $(bsd-instantiate "$shellDotBsd" -A shellDrv) --run \
    'echo "$IMPURE_VAR - $VAR_FROM_STDENV_SETUP - $VAR_FROM_NIX - $TEST_inBsdShell"') = " - foo - bar - false" ]]

[[ $(bsd-shell --pure $(bsd-instantiate "$shellDotBsd" -A shellDrv) --run \
    'echo "$IMPURE_VAR - $VAR_FROM_STDENV_SETUP - $VAR_FROM_NIX - $TEST_inBsdShell"') = " - foo - bar - false" ]]

# Test bsd-shell on a .drv symlink

# Legacy: absolute path and .drv extension required
bsd-instantiate "$shellDotBsd" -A shellDrv --add-root $TEST_ROOT/shell.drv
[[ $(bsd-shell --pure $TEST_ROOT/shell.drv --run \
    'echo "$IMPURE_VAR - $VAR_FROM_STDENV_SETUP - $VAR_FROM_NIX"') = " - foo - bar" ]]

# New behaviour: just needs to resolve to a derivation in the store
bsd-instantiate "$shellDotBsd" -A shellDrv --add-root $TEST_ROOT/shell
[[ $(bsd-shell --pure $TEST_ROOT/shell --run \
    'echo "$IMPURE_VAR - $VAR_FROM_STDENV_SETUP - $VAR_FROM_NIX"') = " - foo - bar" ]]

# Test bsd-shell -p
output=$(NIX_PATH=bsdpkgs="$shellDotBsd" bsd-shell --pure -p foo bar --run 'echo "$(foo) $(bar)"')
[ "$output" = "foo bar" ]

# Test bsd-shell -p --arg x y
output=$(NIX_PATH=bsdpkgs="$shellDotBsd" bsd-shell --pure -p foo --argstr fooContents baz --run 'echo "$(foo)"')
[ "$output" = "baz" ]

# Test bsd-shell shebang mode
sed -e "s|@ENV_PROG@|$(type -P env)|" shell.shebang.sh > $TEST_ROOT/shell.shebang.sh
chmod a+rx $TEST_ROOT/shell.shebang.sh

output=$($TEST_ROOT/shell.shebang.sh abc def)
[ "$output" = "foo bar abc def" ]

# Test bsd-shell shebang mode with an alternate working directory
sed -e "s|@ENV_PROG@|$(type -P env)|" shell.shebang.expr > $TEST_ROOT/shell.shebang.expr
chmod a+rx $TEST_ROOT/shell.shebang.expr
# Should fail due to expressions using relative path
! $TEST_ROOT/shell.shebang.expr bar
cp shell.bsd "${config_bsd}" $TEST_ROOT
# Should succeed
echo "cwd: $PWD"
output=$($TEST_ROOT/shell.shebang.expr bar)
[ "$output" = foo ]

# Test bsd-shell shebang mode with an alternate working directory
sed -e "s|@ENV_PROG@|$(type -P env)|" shell.shebang.legacy.expr > $TEST_ROOT/shell.shebang.legacy.expr
chmod a+rx $TEST_ROOT/shell.shebang.legacy.expr
# Should fail due to expressions using relative path
mkdir -p "$TEST_ROOT/somewhere-unrelated"
output="$(cd "$TEST_ROOT/somewhere-unrelated"; $TEST_ROOT/shell.shebang.legacy.expr bar;)"
[[ $(realpath "$output") = $(realpath "$TEST_ROOT/somewhere-unrelated") ]]

# Test bsd-shell shebang mode again with metacharacters in the filename.
# First word of filename is chosen to not match any file in the test root.
sed -e "s|@ENV_PROG@|$(type -P env)|" shell.shebang.sh > $TEST_ROOT/spaced\ \\\'\"shell.shebang.sh
chmod a+rx $TEST_ROOT/spaced\ \\\'\"shell.shebang.sh

output=$($TEST_ROOT/spaced\ \\\'\"shell.shebang.sh abc def)
[ "$output" = "foo bar abc def" ]

# Test bsd-shell shebang mode for ruby
# This uses a fake interpreter that returns the arguments passed
# This, in turn, verifies the `rc` script is valid and the `load()` script (given using `-e`) is as expected.
sed -e "s|@SHELL_PROG@|$(type -P bsd-shell)|" shell.shebang.rb > $TEST_ROOT/shell.shebang.rb
chmod a+rx $TEST_ROOT/shell.shebang.rb

output=$($TEST_ROOT/shell.shebang.rb abc ruby)
[ "$output" = '-e load(ARGV.shift) -- '"$TEST_ROOT"'/shell.shebang.rb abc ruby' ]

# Test bsd-shell shebang mode for ruby again with metacharacters in the filename.
# Note: fake interpreter only space-separates args without adding escapes to its output.
sed -e "s|@SHELL_PROG@|$(type -P bsd-shell)|" shell.shebang.rb > $TEST_ROOT/spaced\ \\\'\"shell.shebang.rb
chmod a+rx $TEST_ROOT/spaced\ \\\'\"shell.shebang.rb

output=$($TEST_ROOT/spaced\ \\\'\"shell.shebang.rb abc ruby)
[ "$output" = '-e load(ARGV.shift) -- '"$TEST_ROOT"'/spaced \'\''"shell.shebang.rb abc ruby' ]

# Test bsd-shell shebang quoting
sed -e "s|@ENV_PROG@|$(type -P env)|" shell.shebang.bsd > $TEST_ROOT/shell.shebang.bsd
chmod a+rx $TEST_ROOT/shell.shebang.bsd
$TEST_ROOT/shell.shebang.bsd

mkdir $TEST_ROOT/lookup-test $TEST_ROOT/empty

echo "import $shellDotBsd" > $TEST_ROOT/lookup-test/shell.bsd
cp "${config_bsd}" $TEST_ROOT/lookup-test/
echo 'abort "do not load default.bsd!"' > $TEST_ROOT/lookup-test/default.bsd

bsd-shell $TEST_ROOT/lookup-test -A shellDrv --run 'echo "it works"' | grepQuiet "it works"
# https://github.com/BasedLinux/bsd/issues/4529
bsd-shell -I "testRoot=$TEST_ROOT" '<testRoot/lookup-test>' -A shellDrv --run 'echo "it works"' | grepQuiet "it works"

expectStderr 1 bsd-shell $TEST_ROOT/lookup-test -A shellDrv --run 'echo "it works"' --option bsd-shell-always-looks-for-shell-bsd false \
  | grepQuiet -F "do not load default.bsd!" # we did, because we chose to enable legacy behavior
expectStderr 1 bsd-shell $TEST_ROOT/lookup-test -A shellDrv --run 'echo "it works"' --option bsd-shell-always-looks-for-shell-bsd false \
  | grepQuiet "Skipping .*lookup-test/shell\.bsd.*, because the setting .*bsd-shell-always-looks-for-shell-bsd.* is disabled. This is a deprecated behavior\. Consider enabling .*bsd-shell-always-looks-for-shell-bsd.*"

(
  cd $TEST_ROOT/empty;
  expectStderr 1 bsd-shell | \
    grepQuiet "error.*no argument specified and no .*shell\.bsd.* or .*default\.bsd.* file found in the working directory"
)

expectStderr 1 bsd-shell -I "testRoot=$TEST_ROOT" '<testRoot/empty>' |
  grepQuiet "error.*neither .*shell\.bsd.* nor .*default\.bsd.* found in .*/empty"

cat >$TEST_ROOT/lookup-test/shebangscript <<EOF
#!$(type -P env) bsd-shell
#!bsd-shell -A shellDrv -i bash
[[ \$VAR_FROM_NIX == bar ]]
echo "script works"
EOF
chmod +x $TEST_ROOT/lookup-test/shebangscript

$TEST_ROOT/lookup-test/shebangscript | grepQuiet "script works"

# https://github.com/BasedLinux/bsd/issues/5431
mkdir $TEST_ROOT/marco{,/polo}
echo 'abort "marco/shell.bsd must not be used, but its mere existence used to cause #5431"' > $TEST_ROOT/marco/shell.bsd
cat >$TEST_ROOT/marco/polo/default.bsd <<EOF
#!$(type -P env) bsd-shell
(import $TEST_ROOT/lookup-test/shell.bsd {}).polo
EOF
chmod a+x $TEST_ROOT/marco/polo/default.bsd
(cd $TEST_ROOT/marco && ./polo/default.bsd | grepQuiet "Polo")

# https://github.com/BasedLinux/bsd/issues/11892
mkdir $TEST_ROOT/issue-11892
cat >$TEST_ROOT/issue-11892/shebangscript <<EOF
#!$(type -P env) bsd-shell
#! bsd-shell -I bsdpkgs=$shellDotBsd
#! bsd-shell -p 'callPackage (import ./my_package.bsd) {}'
#! bsd-shell -i bash
set -euxo pipefail
my_package
EOF
cat >$TEST_ROOT/issue-11892/my_package.bsd <<EOF
{ stdenv, shell, ... }:
stdenv.mkDerivation {
  name = "my_package";
  buildCommand = ''
    mkdir -p \$out/bin
    ( echo "#!\${shell}"
      echo "echo 'ok' 'baz11892'"
    ) > \$out/bin/my_package
    cat \$out/bin/my_package
    chmod a+x \$out/bin/my_package
  '';
}
EOF
chmod a+x $TEST_ROOT/issue-11892/shebangscript
$TEST_ROOT/issue-11892/shebangscript \
  | tee /dev/stderr \
  | grepQuiet "ok baz11892"


#####################
# Flake equivalents #
#####################

# Test 'bsd develop'.
bsd develop -f "$shellDotBsd" shellDrv -c bash -c '[[ -n $stdenv ]]'

# Ensure `bsd develop -c` preserves stdin
echo foo | bsd develop -f "$shellDotBsd" shellDrv -c cat | grepQuiet foo

# Ensure `bsd develop -c` actually executes the command if stdout isn't a terminal
bsd develop -f "$shellDotBsd" shellDrv -c echo foo |& grepQuiet foo

# Test 'bsd print-dev-env'.

bsd print-dev-env -f "$shellDotBsd" shellDrv > $TEST_ROOT/dev-env.sh
bsd print-dev-env -f "$shellDotBsd" shellDrv --json > $TEST_ROOT/dev-env.json

# Test with raw drv

shellDrv=$(bsd-instantiate "$shellDotBsd" -A shellDrv.out)

bsd develop $shellDrv -c bash -c '[[ -n $stdenv ]]'

bsd print-dev-env $shellDrv > $TEST_ROOT/dev-env2.sh
bsd print-dev-env $shellDrv --json > $TEST_ROOT/dev-env2.json

diff $TEST_ROOT/dev-env{,2}.sh
diff $TEST_ROOT/dev-env{,2}.json

# Ensure `bsd print-dev-env --json` contains variable assignments.
[[ $(jq -r .variables.arr1.value[2] $TEST_ROOT/dev-env.json) = '3 4' ]]

# Run tests involving `source <(bsd print-dev-env)` in subshells to avoid modifying the current
# environment.

set -u

# Ensure `source <(bsd print-dev-env)` modifies the environment.
(
    path=$PATH
    source $TEST_ROOT/dev-env.sh
    [[ -n $stdenv ]]
    [[ ${arr1[2]} = "3 4" ]]
    [[ ${arr2[1]} = $'\n' ]]
    [[ ${arr2[2]} = $'x\ny' ]]
    [[ $(fun) = blabla ]]
    [[ $PATH = $(jq -r .variables.PATH.value $TEST_ROOT/dev-env.json):$path ]]
)

# Ensure `source <(bsd print-dev-env)` handles the case when PATH is empty.
(
    path=$PATH
    PATH=
    source $TEST_ROOT/dev-env.sh
    [[ $PATH = $(PATH=$path jq -r .variables.PATH.value $TEST_ROOT/dev-env.json) ]]
)

# Test bsd-shell with ellipsis and no `inBsdShell` argument (for backwards compat with old bsdpkgs)
cat >$TEST_ROOT/shell-ellipsis.bsd <<EOF
{ system ? "x86_64-linux", ... }@args:
assert (!(args ? inBsdShell));
(import $shellDotBsd { }).shellDrv
EOF
bsd-shell $TEST_ROOT/shell-ellipsis.bsd --run "true"
