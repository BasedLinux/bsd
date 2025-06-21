let

  overrides = {
    import = fn: scopedImport overrides fn;

    scopedImport = attrs: fn: scopedImport (overrides // attrs) fn;

    builtins = builtins // overrides;
  } // import ./lib.bsd;

in
scopedImport overrides ./imported.bsd
