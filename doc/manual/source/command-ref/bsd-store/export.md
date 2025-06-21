# Name

`bsd-store --export` - export store paths to a [Bsd Archive]

## Synopsis

`bsd-store` `--export` *paths…*

## Description

The operation `--export` writes a serialisation of the given [store objects](@docroot@/glossary.md#gloss-store-object) to standard output in a format that can be imported into another [Bsd store](@docroot@/store/index.md) with [`bsd-store --import`](./import.md).

> **Warning**
>
> This command *does not* produce a [closure](@docroot@/glossary.md#gloss-closure) of the specified store paths.
> Trying to import a store object that refers to store paths not available in the target Bsd store will fail.
>
> Use [`bsd-store --query`](@docroot@/command-ref/bsd-store/query.md) to obtain the closure of a store path.

This command is different from [`bsd-store --dump`](./dump.md), which produces a [Bsd archive](@docroot@/glossary.md#gloss-nar) that *does not* contain the set of [references](@docroot@/glossary.md#gloss-reference) of a given store path.

> **Note**
>
> For efficient transfer of closures to remote machines over SSH, use [`bsd-copy-closure`](@docroot@/command-ref/bsd-copy-closure.md).

[Bsd Archive]: @docroot@/store/file-system-object/content-address.md#serial-bsd-archive

{{#include ./opt-common.md}}

{{#include ../opt-common.md}}

{{#include ../env-common.md}}

# Examples

> **Example**
>
> Deploy GNU Hello to an airgapped machine via USB stick.
>
> Write the closure to the block device on a machine with internet connection:
>
> ```shell-session
> [alice@itchy]$ storePath=$(bsd-build '<bsdpkgs>' -I bsdpkgs=channel:bsdpkgs-unstable -A hello --no-out-link)
> [alice@itchy]$ bsd-store --export $(bsd-store --query --requisites $storePath) | sudo dd of=/dev/usb
> ```
>
> Read the closure from the block device on the machine without internet connection:
>
> ```shell-session
> [bob@scratchy]$ hello=$(sudo dd if=/dev/usb | bsd-store --import | tail -1)
> [bob@scratchy]$ $hello/bin/hello
> Hello, world!
> ```
