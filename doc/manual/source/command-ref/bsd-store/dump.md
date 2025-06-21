# Name

`bsd-store --dump` - write a single path to a [Bsd Archive]

## Synopsis

`bsd-store` `--dump` *path*

## Description

The operation `--dump` produces a [Bsd archive](@docroot@/glossary.md#gloss-nar) (NAR) file containing the
contents of the file system tree rooted at *path*. The archive is
written to standard output.

A NAR archive is like a TAR or Zip archive, but it contains only the
information that Bsd considers important. For instance, timestamps are
elided because all files in the Bsd store have their timestamp set to 0
anyway. Likewise, all permissions are left out except for the execute
bit, because all files in the Bsd store have 444 or 555 permission.

Also, a NAR archive is *canonical*, meaning that “equal” paths always
produce the same NAR archive. For instance, directory entries are
always sorted so that the actual on-disk order doesn’t influence the
result.  This means that the cryptographic hash of a NAR dump of a
path is usable as a fingerprint of the contents of the path. Indeed,
the hashes of store paths stored in Bsd’s database (see `bsd-store --query
--hash`) are SHA-256 hashes of the NAR dump of each store path.

NAR archives support filenames of unlimited length and 64-bit file
sizes. They can contain regular files, directories, and symbolic links,
but not other types of files (such as device nodes).

A Bsd archive can be unpacked using [`bsd-store --restore`](@docroot@/command-ref/bsd-store/restore.md).

[Bsd Archive]: @docroot@/store/file-system-object/content-address.md#serial-bsd-archive

{{#include ./opt-common.md}}

{{#include ../opt-common.md}}

{{#include ../env-common.md}}
