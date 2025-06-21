R""(

# Examples

* Create a new flake:

  ```console
  # bsd flake new hello
  # cd hello
  ```

* Build the flake in the current directory:

  ```console
  # bsd build
  # ./result/bin/hello
  Hello, world!
  ```

* Run the flake in the current directory:

  ```console
  # bsd run
  Hello, world!
  ```

* Start a development shell for hacking on this flake:

  ```console
  # bsd develop
  # unpackPhase
  # cd hello-*
  # configurePhase
  # buildPhase
  # ./hello
  Hello, world!
  # installPhase
  # ../outputs/out/bin/hello
  Hello, world!
  ```

# Description

Bsd is a tool for building software, configurations and other
artifacts in a reproducible and declarative way. For more information,
see the [Bsd homepage](https://basedlinux.org/) or the [Bsd
manual](https://basedlinux.org/manual/bsd/stable/).

# Installables

> **Warning** \
> Installables are part of the unstable
> [`bsd-command` experimental feature](@docroot@/development/experimental-features.md#xp-feature-bsd-command),
> and subject to change without notice.

Many `bsd` subcommands operate on one or more *installables*.
These are command line arguments that represent something that can be realised in the Bsd store.

The following types of installable are supported by most commands:

- [Flake output attribute](#flake-output-attribute) (experimental)
  - This is the default
- [Store path](#store-path)
  - This is assumed if the argument is a Bsd store path or a symlink to a Bsd store path
- [Bsd file](#bsd-file), optionally qualified by an attribute path
  - Specified with `--file`/`-f`
- [Bsd expression](#bsd-expression), optionally qualified by an attribute path
  - Specified with `--expr`

For most commands, if no installable is specified, `.` is assumed.
That is, Bsd will operate on the default flake output attribute of the flake in the current directory.

### Flake output attribute

> **Warning** \
> Flake output attribute installables depend on both the
> [`flakes`](@docroot@/development/experimental-features.md#xp-feature-flakes)
> and
> [`bsd-command`](@docroot@/development/experimental-features.md#xp-feature-bsd-command)
> experimental features, and subject to change without notice.

Example: `bsdpkgs#hello`

These have the form *flakeref*[`#`*attrpath*], where *flakeref* is a
[flake reference](./bsd3-flake.md#flake-references) and *attrpath* is an optional attribute path. For
more information on flakes, see [the `bsd flake` manual
page](./bsd3-flake.md).  Flake references are most commonly a flake
identifier in the flake registry (e.g. `bsdpkgs`), or a raw path
(e.g. `/path/to/my-flake` or `.` or `../foo`), or a full URL
(e.g. `github:bsdos/bsdpkgs` or `path:.`)

When the flake reference is a raw path (a path without any URL
scheme), it is interpreted as a `path:` or `git+file:` url in the following
way:

- If the path is within a Git repository, then the url will be of the form
  `git+file://[GIT_REPO_ROOT]?dir=[RELATIVE_FLAKE_DIR_PATH]`
  where `GIT_REPO_ROOT` is the path to the root of the git repository,
  and `RELATIVE_FLAKE_DIR_PATH` is the path (relative to the directory
  root) of the closest parent of the given path that contains a `flake.bsd` within
  the git repository.
  If no such directory exists, then Bsd will error-out.

  Note that the search will only include files indexed by git. In particular, files
  which are matched by `.gitignore` or have never been `git add`-ed will not be
  available in the flake. If this is undesirable, specify `path:<directory>` explicitly;

  For example, if `/foo/bar` is a git repository with the following structure:

  ```
  .
  └── baz
      ├── blah
      │   └── file.txt
      └── flake.bsd
  ```

  Then `/foo/bar/baz/blah` will resolve to `git+file:///foo/bar?dir=baz`

- If the supplied path is not a git repository, then the url will have the form
  `path:FLAKE_DIR_PATH` where `FLAKE_DIR_PATH` is the closest parent
  of the supplied path that contains a `flake.bsd` file (within the same file-system).
  If no such directory exists, then Bsd will error-out.

  For example, if `/foo/bar/flake.bsd` exists, then `/foo/bar/baz/` will resolve to
 `path:/foo/bar`

If *attrpath* is omitted, Bsd tries some default values; for most
subcommands, the default is `packages.`*system*`.default`
(e.g. `packages.x86_64-linux.default`), but some subcommands have
other defaults. If *attrpath* *is* specified, *attrpath* is
interpreted as relative to one or more prefixes; for most
subcommands, these are `packages.`*system*,
`legacyPackages.*system*` and the empty prefix. Thus, on
`x86_64-linux` `bsd build bsdpkgs#hello` will try to build the
attributes `packages.x86_64-linux.hello`,
`legacyPackages.x86_64-linux.hello` and `hello`.

If *attrpath* begins with `.` then no prefixes or defaults are attempted. This allows the form *flakeref*[`#.`*attrpath*], such as `github:BasedLinux/bsdpkgs#.lib.fakeSha256` to avoid a search of `packages.*system*.lib.fakeSha256`

### Store path

Example: `/bsd/store/v5sv61sszx301i0x6xysaqzla09nksnd-hello-2.10`

These are paths inside the Bsd store, or symlinks that resolve to a path in the Bsd store.

A [store derivation] is also addressed by store path.

Example: `/bsd/store/p7gp6lxdg32h4ka1q398wd9r2zkbbz2v-hello-2.10.drv`

If you want to refer to an output path of that store derivation, add the output name preceded by a caret (`^`).

Example: `/bsd/store/p7gp6lxdg32h4ka1q398wd9r2zkbbz2v-hello-2.10.drv^out`

All outputs can be referred to at once with the special syntax `^*`.

Example: `/bsd/store/p7gp6lxdg32h4ka1q398wd9r2zkbbz2v-hello-2.10.drv^*`

### Bsd file

Example: `--file /path/to/bsdpkgs hello`

When the option `-f` / `--file` *path* \[*attrpath*...\] is given, installables are interpreted as the value of the expression in the Bsd file at *path*.
If attribute paths are provided, commands will operate on the corresponding values accessible at these paths.
The Bsd expression in that file, or any selected attribute, must evaluate to a derivation.

### Bsd expression

Example: `--expr 'import <bsdpkgs> {}' hello`

When the option `--expr` *expression* \[*attrpath*...\] is given, installables are interpreted as the value of the of the Bsd expression.
If attribute paths are provided, commands will operate on the corresponding values accessible at these paths.
The Bsd expression, or any selected attribute, must evaluate to a derivation.

You may need to specify `--impure` if the expression references impure inputs (such as `<bsdpkgs>`).

## Derivation output selection

Derivations can have multiple outputs, each corresponding to a
different store path. For instance, a package can have a `bin` output
that contains programs, and a `dev` output that provides development
artifacts like C/C++ header files. The outputs on which `bsd` commands
operate are determined as follows:

* You can explicitly specify the desired outputs using the syntax *installable*`^`*output1*`,`*...*`,`*outputN* — that is, a caret followed immediately by a comma-separated list of derivation outputs to select.
  For installables specified as [Flake output attributes](#flake-output-attribute) or [Store paths](#store-path), the output is specified in the same argument:

  For example, you can obtain the `dev` and `static` outputs of the `glibc` package:

  ```console
  # bsd build 'bsdpkgs#glibc^dev,static'
  # ls ./result-dev/include/ ./result-static/lib/
  …
  ```

  and likewise, using a store path to a "drv" file to specify the derivation:

  ```console
  # bsd build '/bsd/store/gzaflydcr6sb3567hap9q6srzx8ggdgg-glibc-2.33-78.drv^dev,static'
  …
  ```

  For `--expr` and `-f`/`--file`, the derivation output is specified as part of the attribute path:

  ```console
  $ bsd build -f '<bsdpkgs>' 'glibc^dev,static'
  $ bsd build --impure --expr 'import <bsdpkgs> { }' 'glibc^dev,static'
  ```

  This syntax is the same even if the actual attribute path is empty:

  ```console
  $ bsd build --impure --expr 'let pkgs = import <bsdpkgs> { }; in pkgs.glibc' '^dev,static'
  ```

* You can also specify that *all* outputs should be used using the
  syntax *installable*`^*`. For example, the following shows the size
  of all outputs of the `glibc` package in the binary cache:

  ```console
  # bsd path-info --closure-size --eval-store auto --store https://cache.basedlinux.org 'bsdpkgs#glibc^*'
  /bsd/store/g02b1lpbddhymmcjb923kf0l7s9nww58-glibc-2.33-123                 33208200
  /bsd/store/851dp95qqiisjifi639r0zzg5l465ny4-glibc-2.33-123-bin             36142896
  /bsd/store/kdgs3q6r7xdff1p7a9hnjr43xw2404z7-glibc-2.33-123-debug          155787312
  /bsd/store/n4xa8h6pbmqmwnq0mmsz08l38abb06zc-glibc-2.33-123-static          42488328
  /bsd/store/q6580lr01jpcsqs4r5arlh4ki2c1m9rv-glibc-2.33-123-dev             44200560
  ```

  and likewise, using a store path to a "drv" file to specify the derivation:

  ```console
  # bsd path-info --closure-size '/bsd/store/gzaflydcr6sb3567hap9q6srzx8ggdgg-glibc-2.33-78.drv^*'
  …
  ```
* If you didn't specify the desired outputs, but the derivation has an
  attribute `meta.outputsToInstall`, Bsd will use those outputs. For
  example, since the package `bsdpkgs#libxml2` has this attribute:

  ```console
  # bsd eval 'bsdpkgs#libxml2.meta.outputsToInstall'
  [ "bin" "man" ]
  ```

  a command like `bsd shell bsdpkgs#libxml2` will provide only those
  two outputs by default.

  Note that a [store derivation] (given by its `.drv` file store path) doesn't have
  any attributes like `meta`, and thus this case doesn't apply to it.

  [store derivation]: @docroot@/glossary.md#gloss-store-derivation

* Otherwise, Bsd will use all outputs of the derivation.

# Bsd stores

Most `bsd` subcommands operate on a *Bsd store*.
The various store types are documented in the
[Store Types](@docroot@/store/types/index.md)
section of the manual.

The same information is also available from the [`bsd help-stores`](./bsd3-help-stores.md) command.

# Shebang interpreter

The `bsd` command can be used as a `#!` interpreter.
Arguments to Bsd can be passed on subsequent lines in the script.

Verbatim strings may be passed in double backtick (```` `` ````) quotes. <!-- that's markdown for two backticks in inline code. -->
Sequences of _n_ backticks of 3 or longer are parsed as _n-1_ literal backticks.
A single space before the closing ```` `` ```` is ignored if present.

`--file` and `--expr` resolve relative paths based on the script location.

Examples:

```
#!/usr/bin/env bsd
#! bsd shell --file ``<bsdpkgs>`` hello cowsay --command bash

hello | cowsay
```

or with **flakes**:

```
#!/usr/bin/env bsd
#! bsd shell bsdpkgs#bash bsdpkgs#hello bsdpkgs#cowsay --command bash

hello | cowsay
```

or with an **expression**:

```bash
#! /usr/bin/env bsd
#! bsd shell --impure --expr ``
#! bsd with (import (builtins.getFlake "bsdpkgs") {});
#! bsd terraform.withPlugins (plugins: [ plugins.openstack ])
#! bsd ``
#! bsd --command bash

terraform "$@"
```

or with cascading interpreters. Note that the `#! bsd` lines don't need to follow after the first line, to accommodate other interpreters.

```
#!/usr/bin/env bsd
//! ```cargo
//! [dependencies]
//! time = "0.1.25"
//! ```
/*
#!bsd shell bsdpkgs#rustc bsdpkgs#rust-script bsdpkgs#cargo --command rust-script
*/
fn main() {
    for argument in std::env::args().skip(1) {
        println!("{}", argument);
    };
    println!("{}", std::env::var("HOME").expect(""));
    println!("{}", time::now().rfc822z());
}
// vim: ft=rust
```

)""
