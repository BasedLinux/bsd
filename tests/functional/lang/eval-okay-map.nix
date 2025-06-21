with import ./lib.bsd;

concat (
  map (x: x + "bar") [
    "foo"
    "bla"
    "xyzzy"
  ]
)
