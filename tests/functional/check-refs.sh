#!/usr/bin/env bash

source common.sh

TODO_BasedLinux

clearStore

RESULT=$TEST_ROOT/result

dep=$(bsd-build -o "$RESULT" check-refs.bsd -A dep)

# test1 references dep, not itself.
test1=$(bsd-build -o "$RESULT" check-refs.bsd -A test1)
bsd-store -q --references "$test1" | grepQuietInverse "$test1"
bsd-store -q --references "$test1" | grepQuiet "$dep"

# test2 references src, not itself nor dep.
test2=$(bsd-build -o "$RESULT" check-refs.bsd -A test2)
bsd-store -q --references "$test2" | grepQuietInverse "$test2"
bsd-store -q --references "$test2" | grepQuietInverse "$dep"
bsd-store -q --references "$test2" | grepQuiet aux-ref

# test3 should fail (unallowed ref).
(! bsd-build -o "$RESULT" check-refs.bsd -A test3)

# test4 should succeed.
bsd-build -o "$RESULT" check-refs.bsd -A test4

# test5 should succeed.
bsd-build -o "$RESULT" check-refs.bsd -A test5

# test6 should fail (unallowed self-ref).
(! bsd-build -o "$RESULT" check-refs.bsd -A test6)

# test7 should succeed (allowed self-ref).
bsd-build -o "$RESULT" check-refs.bsd -A test7

# test8 should fail (toFile depending on derivation output).
(! bsd-build -o "$RESULT" check-refs.bsd -A test8)

# test9 should fail (disallowed reference).
(! bsd-build -o "$RESULT" check-refs.bsd -A test9)

# test10 should succeed (no disallowed references).
bsd-build -o "$RESULT" check-refs.bsd -A test10

if ! isTestOnBasedLinux; then
    # If we have full control over our store, we can test some more things.

    if isDaemonNewer 2.12pre20230103; then
        if ! isDaemonNewer 2.16.0; then
            enableFeatures discard-references
            restartDaemon
        fi

        # test11 should succeed.
        test11=$(bsd-build -o "$RESULT" check-refs.bsd -A test11)
        [[ -z $(bsd-store -q --references "$test11") ]]
    fi

fi

if isDaemonNewer "2.28pre20241225"; then
    # test12 should fail (syntactically invalid).
    expectStderr 1 bsd-build -vvv -o "$RESULT" check-refs.bsd -A test12 >"$TEST_ROOT/test12.stderr"
    grepQuiet -F "output check for 'lib' contains an illegal reference specifier 'dev', expected store path or output name (one of [lib, out])" < "$TEST_ROOT/test12.stderr"
fi
