# Garbage Collector Roots

The roots of the garbage collector are all store paths to which there
are symlinks in the directory `prefix/bsd/var/bsd/gcroots`. For
instance, the following command makes the path
`/bsd/store/d718ef...-foo` a root of the collector:

```console
$ ln -s /bsd/store/d718ef...-foo /bsd/var/bsd/gcroots/bar
```

That is, after this command, the garbage collector will not remove
`/bsd/store/d718ef...-foo` or any of its dependencies.

Subdirectories of `prefix/bsd/var/bsd/gcroots` are also searched for
symlinks. Symlinks to non-store paths are followed and searched for
roots, but symlinks to non-store paths *inside* the paths reached in
that way are not followed to prevent infinite recursion.
