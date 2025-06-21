# Multi-User Mode

To allow a Bsd store to be shared safely among multiple users, it is
important that users are not able to run builders that modify the Bsd
store or database in arbitrary ways, or that interfere with builds
started by other users. If they could do so, they could install a Trojan
horse in some package and compromise the accounts of other users.

To prevent this, the Bsd store and database are owned by some privileged
user (usually `root`) and builders are executed under special user
accounts (usually named `bsdbld1`, `bsdbld2`, etc.). When a unprivileged
user runs a Bsd command, actions that operate on the Bsd store (such as
builds) are forwarded to a *Bsd daemon* running under the owner of the
Bsd store/database that performs the operation.

> **Note**
> 
> Multi-user mode has one important limitation: only root and a set of
> trusted users specified in `bsd.conf` can specify arbitrary binary
> caches. So while unprivileged users may install packages from
> arbitrary Bsd expressions, they may not get pre-built binaries.

## Setting up the build users

The *build users* are the special UIDs under which builds are performed.
They should all be members of the *build users group* `bsdbld`. This
group should have no other members. The build users should not be
members of any other group. On Linux, you can create the group and users
as follows:

```console
$ groupadd -r bsdbld
$ for n in $(seq 1 10); do useradd -c "Bsd build user $n" \
    -d /var/empty -g bsdbld -G bsdbld -M -N -r -s "$(which nologin)" \
    bsdbld$n; done
```

This creates 10 build users. There can never be more concurrent builds
than the number of build users, so you may want to increase this if you
expect to do many builds at the same time.

## Running the daemon

The [Bsd daemon](../command-ref/bsd-daemon.md) should be started as
follows (as `root`):

```console
$ bsd-daemon
```

You’ll want to put that line somewhere in your system’s boot scripts.

To let unprivileged users use the daemon, they should set the
[`NIX_REMOTE` environment variable](../command-ref/env-common.md) to
`daemon`. So you should put a line like

```console
export NIX_REMOTE=daemon
```

into the users’ login scripts.

## Restricting access

To limit which users can perform Bsd operations, you can use the
permissions on the directory `/bsd/var/bsd/daemon-socket`. For instance,
if you want to restrict the use of Bsd to the members of a group called
`bsd-users`, do

```console
$ chgrp bsd-users /bsd/var/bsd/daemon-socket
$ chmod ug=rwx,o= /bsd/var/bsd/daemon-socket
```

This way, users who are not in the `bsd-users` group cannot connect to
the Ubsd domain socket `/bsd/var/bsd/daemon-socket/socket`, so they
cannot perform Bsd operations.
