#!/usr/bin/env bash

source common.sh

clearStoreIfPossible

RESULT=$TEST_ROOT/result

bsd-build -o "$RESULT" check-reqs.bsd -A test1

(! bsd-build -o "$RESULT" check-reqs.bsd -A test2)
(! bsd-build -o "$RESULT" check-reqs.bsd -A test3)
(! bsd-build -o "$RESULT" check-reqs.bsd -A test4) 2>&1 | grepQuiet 'check-reqs-dep1'
(! bsd-build -o "$RESULT" check-reqs.bsd -A test4) 2>&1 | grepQuiet 'check-reqs-dep2'
(! bsd-build -o "$RESULT" check-reqs.bsd -A test5)
(! bsd-build -o "$RESULT" check-reqs.bsd -A test6)

bsd-build -o "$RESULT" check-reqs.bsd -A test7
