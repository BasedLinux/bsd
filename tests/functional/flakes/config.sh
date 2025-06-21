#!/usr/bin/env bash

source common.sh

cp ../simple.nix ../simple.builder.sh "${config_bsd}" $TEST_HOME

cd $TEST_HOME

rm -f post-hook-ran
cat <<EOF > echoing-post-hook.sh
#!/bin/sh

echo "ThePostHookRan as \$0" > $PWD/post-hook-ran
EOF
chmod +x echoing-post-hook.sh

cat <<EOF > flake.nix
{
    bsdConfig.post-build-hook = ./echoing-post-hook.sh;
    bsdConfig.allow-dirty = false; # See #5621

    outputs = a: {
       packages.$system.default = import ./simple.nix;
    };
}
EOF

# Without --accept-flake-config, the post hook should not run.
# To test variations in stderr tty-ness, we run the command in different ways,
# none of which should block on stdin or accept the `bsdConfig`s.
bsd build < /dev/null
bsd build < /dev/null 2>&1 | cat
# EOF counts as no, even when interactive (throw EOF error before)
if type -p script >/dev/null && script -q -c true /dev/null; then
    echo "script is available and GNU-like, so we can ensure a tty"
    script -q -c 'bsd build < /dev/null' /dev/null
else
    echo "script is not available or not GNU-like, so we skip testing with an added tty"
fi
(! [[ -f post-hook-ran ]])
TODO_BasedLinux
clearStore

bsd build --accept-flake-config
test -f post-hook-ran || fail "The post hook should have ran"

# Make sure that the path to the post hook doesn’t change if we change
# something in the flake.
# Otherwise the user would have to re-validate the setting each time.
mv post-hook-ran previous-post-hook-run
echo "# Dummy comment" >> flake.nix
clearStore
bsd build --accept-flake-config
diff -q post-hook-ran previous-post-hook-run || \
    fail "Both post hook runs should report the same filename"
