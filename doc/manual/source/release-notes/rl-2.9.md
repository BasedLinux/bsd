# Release 2.9 (2022-05-30)

* Running Bsd with the new `--debugger` flag will cause it to start a
  repl session if an exception is thrown during evaluation, or if
  `builtins.break` is called.  From there you can inspect the values
  of variables and evaluate Bsd expressions.  In debug mode, the
  following new repl commands are available:

  ```
  :env          Show env stack
  :bt           Show trace stack
  :st           Show current trace
  :st <idx>     Change to another trace in the stack
  :c            Go until end of program, exception, or builtins.break().
  :s            Go one step
  ```

  Read more about the debugger
  [here](https://www.zknotes.com/note/5970).

* Bsd now provides better integration with zsh's `run-help`
  feature. It is now included in the Bsd installation in the form of
  an autoloadable shell function, `run-help-bsd`. It picks up Bsd
  subcommands from the currently typed in command and directs the user
  to the associated man pages.

* `bsd repl` has a new build-and-link (`:bl`) command that builds a
  derivation while creating GC root symlinks.

* The path produced by `builtins.toFile` is now allowed to be imported
  or read even with restricted evaluation. Note that this will not
  work with a read-only store.

* `bsd build` has a new `--print-out-paths` flag to print the
  resulting output paths.  This matches the default behaviour of
  `bsd-build`.

* You can now specify which outputs of a derivation `bsd` should
  operate on using the syntax `installable^outputs`,
  e.g. `bsdpkgs#glibc^dev,static` or `bsdpkgs#glibc^*`. By default,
  `bsd` will use the outputs specified by the derivation's
  `meta.outputsToInstall` attribute if it exists, or all outputs
  otherwise.

* `builtins.fetchTree` (and flake inputs) can now be used to fetch
  plain files over the `http(s)` and `file` protocols in addition to
  directory tarballs.
