# Name

`bsd-build` - build a Bsd expression

# Synopsis

`bsd-build` [*paths…*]
  [`--arg` *name* *value*]
  [`--argstr` *name* *value*]
  [{`--attr` | `-A`} *attrPath*]
  [`--no-out-link`]
  [`--dry-run`]
  [{`--out-link` | `-o`} *outlink*]

# Disambiguation

This man page describes the command `bsd-build`, which is distinct from `bsd
build`. For documentation on the latter, run `bsd build --help` or see `man
bsd3-build`.

# Description

The `bsd-build` command builds the derivations described by the Bsd
expressions in *paths*. If the build succeeds, it places a symlink to
the result in the current directory. The symlink is called `result`. If
there are multiple Bsd expressions, or the Bsd expressions evaluate to
multiple derivations, multiple sequentially numbered symlinks are
created (`result`, `result-2`, and so on).

If no *paths* are specified, then `bsd-build` will use `default.bsd` in
the current directory, if it exists.

If an element of *paths* starts with `http://` or `https://`, it is
interpreted as the URL of a tarball that will be downloaded and unpacked
to a temporary location. The tarball must include a single top-level
directory containing at least a file named `default.bsd`.

`bsd-build` is essentially a wrapper around
[`bsd-instantiate`](bsd-instantiate.md) (to translate a high-level Bsd
expression to a low-level [store derivation]) and [`bsd-store
--realise`](@docroot@/command-ref/bsd-store/realise.md) (to build the store
derivation).

[store derivation]: @docroot@/glossary.md#gloss-store-derivation

> **Warning**
>
> The result of the build is automatically registered as a root of the
> Bsd garbage collector. This root disappears automatically when the
> `result` symlink is deleted or renamed. So don’t rename the symlink.

# Options

All options not listed here are passed to
[`bsd-store --realise`](bsd-store/realise.md),
except for `--arg` and `--attr` / `-A` which are passed to [`bsd-instantiate`](bsd-instantiate.md).

- <span id="opt-no-out-link">[`--no-out-link`](#opt-no-out-link)<span>

  Do not create a symlink to the output path. Note that as a result
  the output does not become a root of the garbage collector, and so
  might be deleted by `bsd-store --gc`.

- <span id="opt-dry-run">[`--dry-run`](#opt-dry-run)</span>

  Show what store paths would be built or downloaded.

- <span id="opt-out-link">[`--out-link`](#opt-out-link)</span> / `-o` *outlink*

  Change the name of the symlink to the output path created from
  `result` to *outlink*.

{{#include ./status-build-failure.md}}

{{#include ./opt-common.md}}

{{#include ./env-common.md}}

# Examples

```console
$ bsd-build '<bsdpkgs>' --attr firefox
store derivation is /bsd/store/qybprl8sz2lc...-firefox-1.5.0.7.drv
/bsd/store/d18hyl92g30l...-firefox-1.5.0.7

$ ls -l result
lrwxrwxrwx  ...  result -> /bsd/store/d18hyl92g30l...-firefox-1.5.0.7

$ ls ./result/bin/
firefox  firefox-config
```

If a derivation has multiple outputs, `bsd-build` will build the default
(first) output. You can also build all outputs:

```console
$ bsd-build '<bsdpkgs>' --attr openssl.all
```

This will create a symlink for each output named `result-outputname`.
The suffix is omitted if the output name is `out`. So if `openssl` has
outputs `out`, `bin` and `man`, `bsd-build` will create symlinks
`result`, `result-bin` and `result-man`. It’s also possible to build a
specific output:

```console
$ bsd-build '<bsdpkgs>' --attr openssl.man
```

This will create a symlink `result-man`.

Build a Bsd expression given on the command line:

```console
$ bsd-build --expr 'with import <bsdpkgs> { }; runCommand "foo" { } "echo bar > $out"'
$ cat ./result
bar
```

Build the GNU Hello package from the latest revision of the master
branch of Bsdpkgs:

```console
$ bsd-build https://github.com/BasedLinux/bsdpkgs/archive/master.tar.gz --attr hello
```
