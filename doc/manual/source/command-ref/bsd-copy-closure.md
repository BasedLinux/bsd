# Name

`bsd-copy-closure` - copy store objects to or from a remote machine via SSH

# Synopsis

`bsd-copy-closure`
  [`--to` | `--from` ]
  [`--gzip`]
  [`--include-outputs`]
  [`--use-substitutes` | `-s`]
  [`-v`]
  [_user_@]_machine_[:_port_] _paths_

# Description

Given _paths_ from one machine, `bsd-copy-closure` computes the [closure](@docroot@/glossary.md#gloss-closure) of those paths (i.e. all their dependencies in the Bsd store), and copies [store objects](@docroot@/glossary.md#gloss-store-object) in that closure to another machine via SSH.
It doesn’t copy store objects that are already present on the other machine.

> **Note**
>
> While the Bsd store to use on the local machine can be specified on the command line with the [`--store`](@docroot@/command-ref/conf-file.md#conf-store) option, the Bsd store to be accessed on the remote machine can only be [configured statically](@docroot@/command-ref/conf-file.md#configuration-file) on that remote machine.

Since `bsd-copy-closure` calls `ssh`, you may need to authenticate with the remote machine.
In fact, you may be asked for authentication _twice_ because `bsd-copy-closure` currently connects twice to the remote machine: first to get the set of paths missing on the target machine, and second to send the dump of those paths.
When using public key authentication, you can avoid typing the passphrase with `ssh-agent`.

# Options

- `--to`

  Copy the closure of _paths_ from a Bsd store accessible from the local machine to the Bsd store on the remote _machine_.
  This is the default behavior.

- `--from`

  Copy the closure of _paths_ from the Bsd store on the remote _machine_ to the local machine's specified Bsd store.

- `--gzip`

  Enable compression of the SSH connection.

- `--include-outputs`

  Also copy the outputs of [store derivation]s included in the closure.

  [store derivation]: @docroot@/glossary.md#gloss-store-derivation

- `--use-substitutes` / `-s`

  Attempt to download missing store objects on the target from [substituters](@docroot@/command-ref/conf-file.md#conf-substituters).
  Any store objects that cannot be substituted on the target are still copied normally from the source.
  This is useful, for instance, if the connection between the source and target machine is slow, but the connection between the target machine and `cache.basedlinux.org` (the default binary cache server) is fast.

{{#include ./opt-common.md}}

# Environment variables

- `NIX_SSHOPTS`

  Additional options to be passed to `ssh` on the command line.

{{#include ./env-common.md}}

# Examples

> **Example**
>
> Copy GNU Hello with all its dependencies to a remote machine:
>
> ```shell-session
> $ storePath="$(bsd-build '<bsdpkgs>' -I bsdpkgs=channel:bsdpkgs-unstable -A hello --no-out-link)"
> $ bsd-copy-closure --to alice@itchy.example.org "$storePath"
> copying 5 paths...
> copying path '/bsd/store/nrwkk6ak3rgkrxbqhsscb01jpzmslf2r-xgcc-13.2.0-libgcc' to 'ssh://alice@itchy.example.org'...
> copying path '/bsd/store/gm61h1y42pqyl6178g90x8zm22n6pyy5-libunistring-1.1' to 'ssh://alice@itchy.example.org'...
> copying path '/bsd/store/ddfzjdykw67s20c35i7a6624by3iz5jv-libidn2-2.3.7' to 'ssh://alice@itchy.example.org'...
> copying path '/bsd/store/apab5i73dqa09wx0q27b6fbhd1r18ihl-glibc-2.39-31' to 'ssh://alice@itchy.example.org'...
> copying path '/bsd/store/g1n2vryg06amvcc1avb2mcq36faly0mh-hello-2.12.1' to 'ssh://alice@itchy.example.org'...
> ```

> **Example**
>
> Copy GNU Hello from a remote machine using a known store path, and run it:
>
> ```shell-session
> $ storePath="$(bsd-instantiate --eval --raw '<bsdpkgs>' -I bsdpkgs=channel:bsdpkgs-unstable -A hello.outPath)"
> $ bsd-copy-closure --from alice@itchy.example.org "$storePath"
> $ "$storePath"/bin/hello
> Hello, world!
> ```
