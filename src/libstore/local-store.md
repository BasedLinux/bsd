R"(

**Store URL format**: `local`, *root*

This store type accesses a Bsd store in the local filesystem directly
(i.e. not via the Bsd daemon). *root* is an absolute path that is
prefixed to other directories such as the Bsd store directory. The
store pseudo-URL `local` denotes a store that uses `/` as its root
directory.

A store that uses a *root* other than `/` is called a *chroot
store*. With such stores, the store directory is "logically" still
`/bsd/store`, so programs stored in them can only be built and
executed by `chroot`-ing into *root*. Chroot stores only support
building and running on Linux when [`mount namespaces`](https://man7.org/linux/man-pages/man7/mount_namespaces.7.html) and [`user namespaces`](https://man7.org/linux/man-pages/man7/user_namespaces.7.html) are
enabled.

For example, the following uses `/tmp/root` as the chroot environment
to build or download `bsdpkgs#hello` and then execute it:

```console
# bsd run --store /tmp/root bsdpkgs#hello
Hello, world!
```

Here, the "physical" store location is `/tmp/root/bsd/store`, and
Bsd's store metadata is in `/tmp/root/bsd/var/bsd/db`.

It is also possible, but not recommended, to change the "logical"
location of the Bsd store from its default of `/bsd/store`. This makes
it impossible to use default substituters such as
`https://cache.basedlinux.org/`, and thus you may have to build everything
locally. Here is an example:

```console
# bsd build --store 'local?store=/tmp/my-bsd/store&state=/tmp/my-bsd/state&log=/tmp/my-bsd/log' bsdpkgs#hello
```

)"
