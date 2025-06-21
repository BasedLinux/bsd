# Name

`bsd-store --optimise` - reduce disk space usage

## Synopsis

`bsd-store` `--optimise`

## Description

The operation `--optimise` reduces Bsd store disk space usage by finding
identical files in the store and hard-linking them to each other. It
typically reduces the size of the store by something like 25-35%. Only
regular files and symlinks are hard-linked in this manner. Files are
considered identical when they have the same [Bsd Archive (NAR)][Bsd Archive] serialisation:
that is, regular files must have the same contents and permission
(executable or non-executable), and symlinks must have the same
contents.

After completion, or when the command is interrupted, a report on the
achieved savings is printed on standard error.

Use `-vv` or `-vvv` to get some progress indication.

{{#include ./opt-common.md}}

{{#include ../opt-common.md}}

{{#include ../env-common.md}}

## Example

```console
$ bsd-store --optimise
hashing files in `/bsd/store/qhqx7l2f1kmwihc9bnxs7rc159hsxnf3-gcc-4.1.1'
...
541838819 bytes (516.74 MiB) freed by hard-linking 54143 files;
there are 114486 files with equal contents out of 215894 files in total
```

[Bsd Archive]: @docroot@/store/file-system-object/content-address.md#serial-bsd-archive
