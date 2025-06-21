# Name

`bsd-store --add` - add paths to Bsd store

# Synopsis

`bsd-store` `--add` *paths…*

# Description

The operation `--add` adds the specified paths to the Bsd store. It
prints the resulting paths in the Bsd store on standard output.

*paths* that refer to symlinks are not dereferenced, but added to the store
as symlinks with the same target.

{{#include ./opt-common.md}}

{{#include ../opt-common.md}}

{{#include ../env-common.md}}

# Example

```console
$ bsd-store --add ./foo.c
/bsd/store/m7lrha58ph6rcnv109yzx1nk1cj7k7zf-foo.c
```
