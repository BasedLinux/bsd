assert
  {
    foo = {
      type = "derivation";
      outPath = "/bsd/store/0";
      ignored = abort "not ignored";
    };
  } == {
    foo = {
      type = "derivation";
      outPath = "/bsd/store/1";
      ignored = abort "not ignored";
    };
  };
throw "unreachable"
