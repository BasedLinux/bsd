#!/usr/bin/env bsd-shell
#!bsd-shell -i python3 -p python3 --pure

# To be used with `--trace-function-calls` and `flamegraph.pl`.
#
# For example:
#
# bsd-instantiate --trace-function-calls '<bsdpkgs>' -A hello 2> bsd-function-calls.trace
# ./contrib/stack-collapse.py bsd-function-calls.trace > bsd-function-calls.folded
# bsd-shell -p flamegraph --run "flamegraph.pl bsd-function-calls.folded > bsd-function-calls.svg"

import sys
from pprint import pprint
import fileinput

stack = []
timestack = []

for line in fileinput.input():
    components = line.strip().split(" ", 2)
    if components[0] != "function-trace":
        continue

    direction = components[1]
    components = components[2].rsplit(" ", 2)

    loc = components[0]
    _at = components[1]
    time = int(components[2])

    if direction == "entered":
        stack.append(loc)
        timestack.append(time)
    elif direction == "exited":
        dur = time - timestack.pop()
        vst = ";".join(stack)
        print(f"{vst} {dur}")
        stack.pop()
