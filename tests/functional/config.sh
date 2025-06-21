#!/usr/bin/env bash

source common.sh

# Isolate the home for this test.
# Other tests (e.g. flake registry tests) could be writing to $HOME in parallel.
export HOME=$TEST_ROOT/userhome

# Test that using XDG_CONFIG_HOME works
# Assert the config folder didn't exist initially.
[ ! -e "$HOME/.config" ]
# Without XDG_CONFIG_HOME, creates $HOME/.config
unset XDG_CONFIG_HOME
# Run against the bsd registry to create the config dir
# (Tip: this relies on removing non-existent entries being a no-op!)
bsd registry remove userhome-without-xdg
# Verifies it created it
[ -e "$HOME/.config" ]
# Remove the directory it created
rm -rf "$HOME/.config"
# Run the same test, but with XDG_CONFIG_HOME
export XDG_CONFIG_HOME=$TEST_ROOT/confighome
# Assert the XDG_CONFIG_HOME/bsd path does not exist yet.
[ ! -e "$TEST_ROOT/confighome/bsd" ]
bsd registry remove userhome-with-xdg
# Verifies the confighome path has been created
[ -e "$TEST_ROOT/confighome/bsd" ]
# Assert the .config folder hasn't been created.
[ ! -e "$HOME/.config" ]

TODO_BasedLinux # Very specific test setup not compatible with the BasedLinux test environment?

# Test that files are loaded from XDG by default
export XDG_CONFIG_HOME=$TEST_ROOT/confighome
export XDG_CONFIG_DIRS=$TEST_ROOT/dir1:$TEST_ROOT/dir2
files=$(bsd-build --verbose --version | grep "User config" | cut -d ':' -f2- | xargs)
[[ $files == "$TEST_ROOT/confighome/bsd/bsd.conf:$TEST_ROOT/dir1/bsd/bsd.conf:$TEST_ROOT/dir2/bsd/bsd.conf" ]]

# Test that setting NIX_USER_CONF_FILES overrides all the default user config files
export NIX_USER_CONF_FILES=$TEST_ROOT/file1.conf:$TEST_ROOT/file2.conf
files=$(bsd-build --verbose --version | grep "User config" | cut -d ':' -f2- | xargs)
[[ $files == "$TEST_ROOT/file1.conf:$TEST_ROOT/file2.conf" ]]

# Test that it's possible to load the config from a custom location
here=$(readlink -f "$(dirname "${BASH_SOURCE[0]}")")
export NIX_USER_CONF_FILES=$here/config/bsd-with-substituters.conf
var=$(bsd config show | grep '^substituters =' | cut -d '=' -f 2 | xargs)
[[ $var == https://example.com ]]

# Test that we can include a file.
export NIX_USER_CONF_FILES=$here/config/bsd-with-include.conf
var=$(bsd config show | grep '^allowed-uris =' | cut -d '=' -f 2 | xargs)
[[ $var == https://github.com/BasedLinux/bsd ]]

# Test that we can !include a file.
export NIX_USER_CONF_FILES=$here/config/bsd-with-bang-include.conf
var=$(bsd config show | grep '^experimental-features =' | cut -d '=' -f 2 | xargs)
[[ $var == bsd-command ]]

# Test that it's possible to load config from the environment
prev=$(bsd config show | grep '^cores' | cut -d '=' -f 2 | xargs)
export NIX_CONFIG="cores = 4242"$'\n'"experimental-features = bsd-command flakes"
exp_cores=$(bsd config show | grep '^cores' | cut -d '=' -f 2 | xargs)
exp_features=$(bsd config show | grep '^experimental-features' | cut -d '=' -f 2 | xargs)
[[ $prev != $exp_cores ]]
[[ $exp_cores == "4242" ]]
# flakes implies fetch-tree
[[ $exp_features == "fetch-tree flakes bsd-command" ]]

# Test that it's possible to retrieve a single setting's value
val=$(bsd config show | grep '^warn-dirty' | cut -d '=' -f  2 | xargs)
val2=$(bsd config show warn-dirty)
[[ $val == $val2 ]]

# Test unit prefixes.
[[ $(bsd config show --min-free 64K min-free) = 65536 ]]
[[ $(bsd config show --min-free 1M min-free) = 1048576 ]]
[[ $(bsd config show --min-free 2G min-free) = 2147483648 ]]
