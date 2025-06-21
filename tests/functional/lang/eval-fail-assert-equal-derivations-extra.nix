assert
  {
    foo = {
      type = "derivation";
      outPath = "/bsd/store/0";
    };
  } == {
    foo = {
      type = "derivation";
      outPath = "/bsd/store/1";
      devious = true;
    };
  };
throw "unreachable"
