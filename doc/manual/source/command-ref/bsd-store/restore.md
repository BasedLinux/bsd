# Name

`bsd-store --restore` - extract a Bsd archive

## Synopsis

`bsd-store` `--restore` *path*

## Description

The operation `--restore` unpacks a [Bsd Archive (NAR)][Bsd Archive] to *path*, which must
not already exist. The archive is read from standard input.

[Bsd Archive]: @docroot@/store/file-system-object/content-address.md#serial-bsd-archive

{{#include ./opt-common.md}}

{{#include ../opt-common.md}}

{{#include ../env-common.md}}
