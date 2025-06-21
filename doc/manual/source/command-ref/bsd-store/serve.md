# Name

`bsd-store --serve` - serve local Bsd store over SSH

# Synopsis

`bsd-store` `--serve` [`--write`]

# Description

The operation `--serve` provides access to the Bsd store over stdin and
stdout, and is intended to be used as a means of providing Bsd store
access to a restricted ssh user.

The following flags are available:

- `--write`

  Allow the connected client to request the realization of
  derivations. In effect, this can be used to make the host act as a
  remote builder.

{{#include ./opt-common.md}}

{{#include ../opt-common.md}}

{{#include ../env-common.md}}

# Examples

To turn a host into a build server, the `authorized_keys` file can be
used to provide build access to a given SSH public key:

```console
$ cat <<EOF >>/root/.ssh/authorized_keys
command="nice -n20 bsd-store --serve --write" ssh-rsa AAAAB3NzaC1yc2EAAAA...
EOF
```

