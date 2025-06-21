with import ./lib.bsd;

builtins.foldl' (x: y: x + y) 0 (range 1 1000)
