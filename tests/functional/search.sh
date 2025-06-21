#!/usr/bin/env bash

source common.sh

clearStoreIfPossible
clearCache

(( $(bsd search -f search.bsd '' hello | wc -l) > 0 ))

# Check descriptions are searched
(( $(bsd search -f search.bsd '' broken | wc -l) > 0 ))

# Check search that matches nothing
(( $(bsd search -f search.bsd '' nosuchpackageexists | wc -l) == 0 ))

# Search for multiple arguments
(( $(bsd search -f search.bsd '' hello empty | wc -l) == 2 ))

# Multiple arguments will not exist
(( $(bsd search -f search.bsd '' hello broken | wc -l) == 0 ))

# No regex should return an error
(( $(bsd search -f search.bsd '' | wc -l) == 0 ))

## Search expressions

# Check that empty search string matches all
bsd search -f search.bsd '' ^ | grepQuiet foo
bsd search -f search.bsd '' ^ | grepQuiet bar
bsd search -f search.bsd '' ^ | grepQuiet hello

## Tests for multiple regex/match highlighting

e=$'\x1b' # grep doesn't support \e, \033 or even \x1b
# Multiple overlapping regexes
(( $(bsd search -f search.bsd '' 'oo' 'foo' 'oo' | grep -c "$e\[32;1mfoo$e\\[0;1m") == 1 ))
(( $(bsd search -f search.bsd '' 'broken b' 'en bar' | grep -c "$e\[32;1mbroken bar$e\\[0m") == 1 ))

# Multiple matches
# Searching for 'o' should yield the 'o' in 'broken bar', the 'oo' in foo and 'o' in hello
(( $(bsd search -f search.bsd '' 'o' | grep -Eoc "$e\[32;1mo{1,2}$e\[(0|0;1)m") == 3 ))
# Searching for 'b' should yield the 'b' in bar and the two 'b's in 'broken bar'
# NOTE: This does not work with `grep -c` because it counts the two 'b's in 'broken bar' as one matched line
(( $(bsd search -f search.bsd '' 'b' | grep -Eo "$e\[32;1mb$e\[(0|0;1)m" | wc -l) == 3 ))

## Tests for --exclude
(( $(bsd search -f search.bsd ^ -e hello | grep -c hello) == 0 ))

(( $(bsd search -f search.bsd foo ^ --exclude 'foo|bar' | grep -Ec 'foo|bar') == 0 ))
(( $(bsd search -f search.bsd foo ^ -e foo --exclude bar | grep -Ec 'foo|bar') == 0 ))
[[ $(bsd search -f search.bsd '' ^ -e bar --json | jq -c 'keys') == '["foo","hello"]' ]]
