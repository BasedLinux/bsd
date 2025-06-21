#!/usr/bin/env bash

# Regression for https://github.com/BasedLinux/bsd/issues/5998 and https://github.com/BasedLinux/bsd/issues/5980

source common.sh

export NIX_PATH=non-existent=/non-existent/but-unused-anyways:by-absolute-path=$PWD:by-relative-path=.

bsd-instantiate --eval -E '<by-absolute-path/simple.nix>' --restrict-eval
bsd-instantiate --eval -E '<by-relative-path/simple.nix>' --restrict-eval

# Should ideally also test this, but there’s no pure way to do it, so just trust me that it works
# bsd-instantiate --eval -E '<bsdpkgs>' -I bsdpkgs=channel:bsdos-unstable --restrict-eval

[[ $(bsd-instantiate --find-file by-absolute-path/simple.nix) = $PWD/simple.nix ]]
[[ $(bsd-instantiate --find-file by-relative-path/simple.nix) = $PWD/simple.nix ]]

# this is the human-readable specification for the following test cases of interactions between various ways of specifying NIX_PATH.
# TODO: the actual tests are incomplete and too manual.
# there should be 43 of them, since the table has 9 rows and columns, and 2 interactions are meaningless
# ideally they would work off the table programmatically.
#
# | precedence             | hard-coded | bsd-path in file | extra-bsd-path in file | bsd-path in env | extra-bsd-path in env | NIX_PATH  | bsd-path  | extra-bsd-path  | -I              |
# |------------------------|------------|------------------|------------------------|-----------------|-----------------------|-----------|-----------|-----------------|-----------------|
# | hard-coded             | x          | ^override        | ^append                | ^override       | ^append               | ^override | ^override | ^append         | ^prepend        |
# | bsd-path in file       |            | last wins        | ^append                | ^override       | ^append               | ^override | ^override | ^append         | ^prepend        |
# | extra-bsd-path in file |            |                  | append in order        | ^override       | ^append               | ^override | ^override | ^append         | ^prepend        |
# | bsd-path in env        |            |                  |                        | last wins       | ^append               | ^override | ^override | ^append         | ^prepend        |
# | extra-bsd-path in env  |            |                  |                        |                 | append in order       | ^override | ^override | ^append         | ^prepend        |
# | NIX_PATH               |            |                  |                        |                 |                       | x         | ^override | ^append         | ^prepend        |
# | bsd-path               |            |                  |                        |                 |                       |           | last wins | ^append         | ^prepend        |
# | extra-bsd-path         |            |                  |                        |                 |                       |           |           | append in order | append in order |
# | -I                     |            |                  |                        |                 |                       |           |           |                 | append in order |

unset NIX_PATH

mkdir -p $TEST_ROOT/{from-bsd-path-file,from-NIX_PATH,from-bsd-path,from-extra-bsd-path,from-I}
for i in from-bsd-path-file from-NIX_PATH from-bsd-path from-extra-bsd-path from-I; do
    touch $TEST_ROOT/$i/only-$i.nix
done

# finding something that's not in any of the default paths fails
( ! $(bsd-instantiate --find-file test) )

echo "bsd-path = test=$TEST_ROOT/from-bsd-path-file" >> "$test_bsd_conf"

# Use bsd.conf in absence of NIX_PATH
[[ $(bsd-instantiate --find-file test) = $TEST_ROOT/from-bsd-path-file ]]

# NIX_PATH overrides bsd.conf
[[ $(NIX_PATH=test=$TEST_ROOT/from-NIX_PATH bsd-instantiate --find-file test) = $TEST_ROOT/from-NIX_PATH ]]
# if NIX_PATH does not have the desired entry, it fails
(! NIX_PATH=test=$TEST_ROOT bsd-instantiate --find-file test/only-from-bsd-path-file.nix)

# -I extends bsd.conf
[[ $(bsd-instantiate -I test=$TEST_ROOT/from-I --find-file test/only-from-I.nix) = $TEST_ROOT/from-I/only-from-I.nix ]]
# if -I does not have the desired entry, the value from bsd.conf is used
[[ $(bsd-instantiate -I test=$TEST_ROOT/from-I --find-file test/only-from-bsd-path-file.nix) = $TEST_ROOT/from-bsd-path-file/only-from-bsd-path-file.nix ]]

# -I extends NIX_PATH
[[ $(NIX_PATH=test=$TEST_ROOT/from-NIX_PATH bsd-instantiate -I test=$TEST_ROOT/from-I --find-file test/only-from-I.nix) = $TEST_ROOT/from-I/only-from-I.nix ]]
# -I takes precedence over NIX_PATH
[[ $(NIX_PATH=test=$TEST_ROOT/from-NIX_PATH bsd-instantiate -I test=$TEST_ROOT/from-I --find-file test) = $TEST_ROOT/from-I ]]
# if -I does not have the desired entry, the value from NIX_PATH is used
[[ $(NIX_PATH=test=$TEST_ROOT/from-NIX_PATH bsd-instantiate -I test=$TEST_ROOT/from-I --find-file test/only-from-NIX_PATH.nix) = $TEST_ROOT/from-NIX_PATH/only-from-NIX_PATH.nix ]]

# --extra-bsd-path extends NIX_PATH
[[ $(NIX_PATH=test=$TEST_ROOT/from-NIX_PATH bsd-instantiate --extra-bsd-path test=$TEST_ROOT/from-extra-bsd-path --find-file test/only-from-extra-bsd-path.nix) = $TEST_ROOT/from-extra-bsd-path/only-from-extra-bsd-path.nix ]]
# if --extra-bsd-path does not have the desired entry, the value from NIX_PATH is used
[[ $(NIX_PATH=test=$TEST_ROOT/from-NIX_PATH bsd-instantiate --extra-bsd-path test=$TEST_ROOT/from-extra-bsd-path --find-file test/only-from-NIX_PATH.nix) = $TEST_ROOT/from-NIX_PATH/only-from-NIX_PATH.nix ]]

# --bsd-path overrides NIX_PATH
[[ $(NIX_PATH=test=$TEST_ROOT/from-NIX_PATH bsd-instantiate --bsd-path test=$TEST_ROOT/from-bsd-path --find-file test) = $TEST_ROOT/from-bsd-path ]]
# if --bsd-path does not have the desired entry, it fails
(! NIX_PATH=test=$TEST_ROOT/from-NIX_PATH bsd-instantiate --bsd-path test=$TEST_ROOT/from-bsd-path --find-file test/only-from-NIX_PATH.nix)

# --bsd-path overrides bsd.conf
[[ $(bsd-instantiate --bsd-path test=$TEST_ROOT/from-bsd-path --find-file test) = $TEST_ROOT/from-bsd-path ]]
(! bsd-instantiate --bsd-path test=$TEST_ROOT/from-bsd-path --find-file test/only-from-bsd-path-file.nix)

# --extra-bsd-path extends bsd.conf
[[ $(bsd-instantiate --extra-bsd-path test=$TEST_ROOT/from-extra-bsd-path --find-file test/only-from-extra-bsd-path.nix) = $TEST_ROOT/from-extra-bsd-path/only-from-extra-bsd-path.nix ]]
# if --extra-bsd-path does not have the desired entry, it is taken from bsd.conf
[[ $(bsd-instantiate --extra-bsd-path test=$TEST_ROOT/from-extra-bsd-path --find-file test) = $TEST_ROOT/from-bsd-path-file ]]

# -I extends --bsd-path
[[ $(bsd-instantiate --bsd-path test=$TEST_ROOT/from-bsd-path -I test=$TEST_ROOT/from-I --find-file test/only-from-I.nix) = $TEST_ROOT/from-I/only-from-I.nix ]]
[[ $(bsd-instantiate --bsd-path test=$TEST_ROOT/from-bsd-path -I test=$TEST_ROOT/from-I --find-file test/only-from-bsd-path.nix) = $TEST_ROOT/from-bsd-path/only-from-bsd-path.nix ]]
