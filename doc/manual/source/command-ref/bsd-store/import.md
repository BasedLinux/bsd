# Name

`bsd-store --import` - import [Bsd Archive] into the store

[Bsd Archive]: @docroot@/store/file-system-object/content-address.md#serial-bsd-archive

# Synopsis

`bsd-store` `--import`

# Description

The operation `--import` reads a serialisation of a set of [store objects](@docroot@/glossary.md#gloss-store-object) produced by [`bsd-store --export`](./export.md) from standard input, and adds those store objects to the specified [Bsd store](@docroot@/store/index.md).
Paths that already exist in the target Bsd store are ignored.
If a path [refers](@docroot@/glossary.md#gloss-reference) to another path that doesn’t exist in the target Bsd store, the import fails.

> **Note**
>
> For efficient transfer of closures to remote machines over SSH, use [`bsd-copy-closure`](@docroot@/command-ref/bsd-copy-closure.md).

{{#include ./opt-common.md}}

{{#include ../opt-common.md}}

{{#include ../env-common.md}}

# Examples

> **Example**
>
> Given a closure of GNU Hello as a file:
>
> ```shell-session
> $ storePath="$(bsd-build '<bsdpkgs>' -I bsdpkgs=channel:bsdpkgs-unstable -A hello --no-out-link)"
> $ bsd-store --export $(bsd-store --query --requisites $storePath) > hello.closure
> ```
>
> Import the closure into a [remote SSH store](@docroot@/store/types/ssh-store.md) using the [`--store`](@docroot@/command-ref/conf-file.md#conf-store) option:
>
> ```console
> $ bsd-store --import --store ssh://alice@itchy.example.org < hello.closure
> ```

