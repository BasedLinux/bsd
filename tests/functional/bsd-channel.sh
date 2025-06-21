#!/usr/bin/env bash

source common.sh

clearProfiles

rm -f $TEST_HOME/.nix-channels $TEST_HOME/.nix-profile

# Test add/list/remove.
bsd-channel --add http://foo/bar xyzzy
bsd-channel --list | grepQuiet http://foo/bar
bsd-channel --remove xyzzy
[[ $(bsd-channel --list-generations | wc -l) == 1 ]]

[ -e $TEST_HOME/.nix-channels ]
[ "$(cat $TEST_HOME/.nix-channels)" = '' ]

# Test the XDG Base Directories support

export NIX_CONFIG="use-xdg-base-directories = true"

bsd-channel --add http://foo/bar xyzzy
bsd-channel --list | grepQuiet http://foo/bar
bsd-channel --remove xyzzy

unset NIX_CONFIG

[ -e $TEST_HOME/.local/state/bsd/channels ]
[ "$(cat $TEST_HOME/.local/state/bsd/channels)" = '' ]

# Create a channel.
rm -rf $TEST_ROOT/foo
mkdir -p $TEST_ROOT/foo
drvPath=$(bsd-instantiate dependencies.nix)
bsd copy --to file://$TEST_ROOT/foo?compression="bzip2" $(bsd-store -r "$drvPath")
rm -rf $TEST_ROOT/bsdexprs
mkdir -p $TEST_ROOT/bsdexprs
cp "${config_bsd}" dependencies.nix dependencies.builder*.sh $TEST_ROOT/bsdexprs/
ln -s dependencies.nix $TEST_ROOT/bsdexprs/default.nix
(cd $TEST_ROOT && tar cvf - bsdexprs) | bzip2 > $TEST_ROOT/foo/bsdexprs.tar.bz2

# Test the update action.
bsd-channel --add file://$TEST_ROOT/foo
bsd-channel --update
[[ $(bsd-channel --list-generations | wc -l) == 2 ]]

# Do a query.
bsd-env -qa \* --meta --xml --out-path > $TEST_ROOT/meta.xml
grepQuiet 'meta.*description.*Random test package' $TEST_ROOT/meta.xml
grepQuiet 'item.*attrPath="foo".*name="dependencies-top"' $TEST_ROOT/meta.xml

# Do an install.
bsd-env -i dependencies-top
[ -e $TEST_HOME/.nix-profile/foobar ]

# Test updating from a tarball
bsd-channel --add file://$TEST_ROOT/foo/bsdexprs.tar.bz2 bar
bsd-channel --update

# Do a query.
bsd-env -qa \* --meta --xml --out-path > $TEST_ROOT/meta.xml
grepQuiet 'meta.*description.*Random test package' $TEST_ROOT/meta.xml
grepQuiet 'item.*attrPath="bar".*name="dependencies-top"' $TEST_ROOT/meta.xml
grepQuiet 'item.*attrPath="foo".*name="dependencies-top"' $TEST_ROOT/meta.xml

# Do an install.
bsd-env -i dependencies-top
[ -e $TEST_HOME/.nix-profile/foobar ]

# Test evaluation through a channel symlink (#9882).
drvPath=$(bsd-instantiate '<foo/dependencies.nix>')

# Add a test for the special case behaviour of 'bsdpkgs' in the
# channels for root (see EvalSettings::getDefaultBsdPath()).
if ! isTestOnBasedLinux; then
    bsd-channel --add file://$TEST_ROOT/foo bsdpkgs
    bsd-channel --update
    mv $TEST_HOME/.local/state/bsd/profiles $TEST_ROOT/var/bsd/profiles/per-user/root
    drvPath2=$(bsd-instantiate '<bsdpkgs>')
    [[ "$drvPath" = "$drvPath2" ]]
fi
