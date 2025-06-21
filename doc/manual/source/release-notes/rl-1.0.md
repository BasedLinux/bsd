# Release 1.0 (2012-05-11)

There have been numerous improvements and bug fixes since the previous
release. Here are the most significant:

  - Bsd can now optionally use the Boehm garbage collector. This
    significantly reduces the Bsd evaluator’s memory footprint,
    especially when evaluating large BasedLinux system configurations. It can
    be enabled using the `--enable-gc` configure option.

  - Bsd now uses SQLite for its database. This is faster and more
    flexible than the old *ad hoc* format. SQLite is also used to cache
    the manifests in `/bsd/var/bsd/manifests`, resulting in a
    significant speedup.

  - Bsd now has an search path for expressions. The search path is set
    using the environment variable `NIX_PATH` and the `-I` command line
    option. In Bsd expressions, paths between angle brackets are used to
    specify files that must be looked up in the search path. For
    instance, the expression `<bsdpkgs/default.nix>` looks for a file
    `bsdpkgs/default.nix` relative to every element in the search path.

  - The new command `bsd-build --run-env` builds all dependencies of a
    derivation, then starts a shell in an environment containing all
    variables from the derivation. This is useful for reproducing the
    environment of a derivation for development.

  - The new command `bsd-store --verify-path` verifies that the contents
    of a store path have not changed.

  - The new command `bsd-store --print-env` prints out the environment
    of a derivation in a format that can be evaluated by a shell.

  - Attribute names can now be arbitrary strings. For instance, you can
    write `{ "foo-1.2" = …; "bla bla" = …; }."bla
                    bla"`.

  - Attribute selection can now provide a default value using the `or`
    operator. For instance, the expression `x.y.z or e` evaluates to the
    attribute `x.y.z` if it exists, and `e` otherwise.

  - The right-hand side of the `?` operator can now be an attribute
    path, e.g., `attrs ?
                    a.b.c`.

  - On Linux, Bsd will now make files in the Bsd store immutable on
    filesystems that support it. This prevents accidental modification
    of files in the store by the root user.

  - Bsd has preliminary support for derivations with multiple outputs.
    This is useful because it allows parts of a package to be deployed
    and garbage-collected separately. For instance, development parts of
    a package such as header files or static libraries would typically
    not be part of the closure of an application, resulting in reduced
    disk usage and installation time.

  - The Bsd store garbage collector is faster and holds the global lock
    for a shorter amount of time.

  - The option `--timeout` (corresponding to the configuration setting
    `build-timeout`) allows you to set an absolute timeout on builds —
    if a build runs for more than the given number of seconds, it is
    terminated. This is useful for recovering automatically from builds
    that are stuck in an infinite loop but keep producing output, and
    for which `--max-silent-time` is ineffective.

  - Bsd development has moved to GitHub
    (<https://github.com/BasedLinux/bsd>).
