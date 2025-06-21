# Release 2.22.0 (2024-04-23)

### Significant changes

- Remove experimental repl-flake [#10103](https://github.com/BasedLinux/bsd/issues/10103) [#10299](https://github.com/BasedLinux/bsd/pull/10299)

  The `repl-flake` experimental feature has been removed. The `bsd repl` command now works like the rest of the new CLI in that `bsd repl {path}` now tries to load a flake at `{path}` (or fails if the `flakes` experimental feature isn't enabled).

### Other changes

- `bsd eval` prints derivations as `.drv` paths [#10200](https://github.com/BasedLinux/bsd/pull/10200)

  `bsd eval` will now print derivations as their `.drv` paths, rather than as
  attribute sets. This makes commands like `bsd eval bsdpkgs#bash` terminate
  instead of infinitely looping into recursive self-referential attributes:

  ```ShellSession
  $ bsd eval bsdpkgs#bash
  «derivation /bsd/store/m32cbgbd598f4w299g0hwyv7gbw6rqcg-bash-5.2p26.drv»
  ```

