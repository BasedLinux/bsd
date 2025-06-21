# Release 2.10 (2022-07-11)

* `bsd repl` now takes installables on the command line, unifying the usage
  with other commands that use `--file` and `--expr`. Primary breaking change
  is for the common usage of `bsd repl '<bsdpkgs>'` which can be recovered with
  `bsd repl --file '<bsdpkgs>'` or `bsd repl --expr 'import <bsdpkgs>{}'`.

  This is currently guarded by the `repl-flake` experimental feature.

* A new function `builtins.traceVerbose` is available. It is similar
  to `builtins.trace` if the `trace-verbose` setting is set to true,
  and it is a no-op otherwise.

* `bsd search` has a new flag `--exclude` to filter out packages.

* On Linux, if `/bsd` doesn't exist and cannot be created and you're
  not running as root, Bsd will automatically use
  `~/.local/share/bsd/root` as a chroot store. This enables non-root
  users to download the statically linked Bsd binary and have it work
  out of the box, e.g.

  ```
  # ~/bsd run bsdpkgs#hello
  warning: '/bsd' does not exists, so Bsd will use '/home/ubuntu/.local/share/bsd/root' as a chroot store
  Hello, world!
  ```

* `flake-registry.json` is now fetched from `channels.basedlinux.org`.

* Bsd can now be built with LTO by passing `--enable-lto` to `configure`.
  LTO is currently only supported when building with GCC.
