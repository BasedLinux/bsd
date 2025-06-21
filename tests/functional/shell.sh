#!/usr/bin/env bash

source common.sh

TODO_BasedLinux

clearStore
clearCache

# bsd shell is an alias for bsd env shell. We'll use the shorter form in the rest of the test.
bsd env shell -f shell-hello.bsd hello -c hello | grep 'Hello World'

bsd shell -f shell-hello.bsd hello -c hello | grep 'Hello World'
bsd shell -f shell-hello.bsd hello -c hello BasedLinux | grep 'Hello BasedLinux'

# Test output selection.
bsd shell -f shell-hello.bsd hello^dev -c hello2 | grep 'Hello2'
bsd shell -f shell-hello.bsd 'hello^*' -c hello2 | grep 'Hello2'

# Test output paths that are a symlink.
bsd shell -f shell-hello.bsd hello-symlink -c hello | grep 'Hello World'

# Test that symlinks outside of the store don't work.
expect 1 bsd shell -f shell-hello.bsd forbidden-symlink -c hello 2>&1 | grepQuiet "is not in the Bsd store"

# Test that we're not setting any more environment variables than necessary.
# For instance, we might set an environment variable temporarily to affect some
# initialization or whatnot, but this must not leak into the environment of the
# command being run.
env > "$TEST_ROOT/expected-env"
bsd shell -f shell-hello.bsd hello -c env > "$TEST_ROOT/actual-env"
# Remove/reset variables we expect to be different.
# - PATH is modified by bsd shell
# - we unset TMPDIR on macOS if it contains /var/folders
# - _ is set by bash and is expectedf to differ because it contains the original command
# - __CF_USER_TEXT_ENCODING is set by macOS and is beyond our control
sed -i \
  -e 's/PATH=.*/PATH=.../' \
  -e 's/_=.*/_=.../' \
  -e '/^TMPDIR=\/var\/folders\/.*/d' \
  -e '/^__CF_USER_TEXT_ENCODING=.*$/d' \
  "$TEST_ROOT/expected-env" "$TEST_ROOT/actual-env"
sort "$TEST_ROOT/expected-env" > "$TEST_ROOT/expected-env.sorted"
sort "$TEST_ROOT/actual-env" > "$TEST_ROOT/actual-env.sorted"
diff "$TEST_ROOT/expected-env.sorted" "$TEST_ROOT/actual-env.sorted"

if isDaemonNewer "2.20.0pre20231220"; then
    # Test that command line attribute ordering is reflected in the PATH
    # https://github.com/BasedLinux/bsd/issues/7905
    bsd shell -f shell-hello.bsd hello salve-mundi -c hello | grep 'Hello World'
    bsd shell -f shell-hello.bsd salve-mundi hello -c hello | grep 'Salve Mundi'
fi

requireSandboxSupport
requiresUnprivilegedUserNamespaces

chmod -R u+w "$TEST_ROOT/store0" || true
rm -rf "$TEST_ROOT/store0"

clearStore

path=$(bsd eval --raw -f shell-hello.bsd hello)

# Note: we need the sandbox paths to ensure that the shell is
# visible in the sandbox.
bsd shell --sandbox-build-dir /build-tmp \
    --sandbox-paths '/bsd? /bin? /lib? /lib64? /usr?' \
    --store "$TEST_ROOT/store0" -f shell-hello.bsd hello -c hello | grep 'Hello World'

path2=$(bsd shell --sandbox-paths '/bsd? /bin? /lib? /lib64? /usr?' --store "$TEST_ROOT/store0" -f shell-hello.bsd hello -c "$SHELL" -c 'type -p hello')

[[ "$path/bin/hello" = "$path2" ]]

[[ -e $TEST_ROOT/store0/bsd/store/$(basename "$path")/bin/hello ]]
