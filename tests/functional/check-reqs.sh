#!/usr/bin/env bash

source common.sh

clearStoreIfPossible

RESULT=$TEST_ROOT/result

bsd-build -o "$RESULT" check-reqs.nix -A test1

(! bsd-build -o "$RESULT" check-reqs.nix -A test2)
(! bsd-build -o "$RESULT" check-reqs.nix -A test3)
(! bsd-build -o "$RESULT" check-reqs.nix -A test4) 2>&1 | grepQuiet 'check-reqs-dep1'
(! bsd-build -o "$RESULT" check-reqs.nix -A test4) 2>&1 | grepQuiet 'check-reqs-dep2'
(! bsd-build -o "$RESULT" check-reqs.nix -A test5)
(! bsd-build -o "$RESULT" check-reqs.nix -A test6)

bsd-build -o "$RESULT" check-reqs.nix -A test7
