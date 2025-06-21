# Name

`bsd-env --rollback` - set user environment to previous generation

# Synopsis

`bsd-env` `--rollback`

# Description

This operation switches to the “previous” generation of the active
profile, that is, the highest numbered generation lower than the current
generation, if it exists. It is just a convenience wrapper around
`--list-generations` and `--switch-generation`.

{{#include ./opt-common.md}}

{{#include ../opt-common.md}}

{{#include ./env-common.md}}

{{#include ../env-common.md}}

# Examples

```console
$ bsd-env --rollback
switching from generation 92 to 91
```

```console
$ bsd-env --rollback
error: no generation older than the current (91) exists
```
