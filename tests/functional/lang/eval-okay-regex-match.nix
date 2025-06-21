with builtins;

let

  matches = pat: s: match pat s != null;

  splitFN = match "((.*)/)?([^/]*)\\.(bsd|cc)";

in

assert matches "foobar" "foobar";
assert matches "fo*" "f";
assert !matches "fo+" "f";
assert matches "fo*" "fo";
assert matches "fo*" "foo";
assert matches "fo+" "foo";
assert matches "fo{1,2}" "foo";
assert !matches "fo{1,2}" "fooo";
assert !matches "fo*" "foobar";
assert matches "[[:space:]]+([^[:space:]]+)[[:space:]]+" "  foo   ";
assert !matches "[[:space:]]+([[:upper:]]+)[[:space:]]+" "  foo   ";

assert match "(.*)\\.bsd" "foobar.bsd" == [ "foobar" ];
assert match "[[:space:]]+([[:upper:]]+)[[:space:]]+" "  FOO   " == [ "FOO" ];

assert
  splitFN "/path/to/foobar.bsd" == [
    "/path/to/"
    "/path/to"
    "foobar"
    "bsd"
  ];
assert
  splitFN "foobar.cc" == [
    null
    null
    "foobar"
    "cc"
  ];

true
