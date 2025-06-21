#!/usr/bin/env bash

# Test that the migration of user environments
# (https://github.com/BasedLinux/bsd/pull/5226) does preserve everything

source common.sh

if isDaemonNewer "2.4pre20211005"; then
    skipTest "Daemon is too new"
fi


killDaemon
unset NIX_REMOTE

TODO_BasedLinux

clearStore
clearProfiles
rm -rf ~/.bsd-profile

# Fill the environment using the older Bsd
PATH_WITH_NEW_NIX="$PATH"
export PATH="$NIX_DAEMON_PACKAGE/bin:$PATH"

bsd-env -f user-envs.bsd -i foo-1.0
bsd-env -f user-envs.bsd -i bar-0.1

# Migrate to the new profile dir, and ensure that everything’s there
export PATH="$PATH_WITH_NEW_NIX"
bsd-env -q # Trigger the migration
( [[ -L ~/.bsd-profile ]] && \
    [[ $(readlink ~/.bsd-profile) == ~/.local/share/bsd/profiles/profile ]] ) || \
    fail "The bsd profile should point to the new location"

(bsd-env -q | grep foo && bsd-env -q | grep bar && \
    [[ -e ~/.bsd-profile/bin/foo ]] && \
    [[ $(bsd-env --list-generations | wc -l) == 2 ]]) ||
    fail "The bsd profile should have the same content as before the migration"
