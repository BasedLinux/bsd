R""(

# Examples

* Delete a specific store path:

  ```console
  # bsd store delete /bsd/store/yb5q57zxv6hgqql42d5r8b5k5mcq6kay-hello-2.10
  ```

# Description

This command deletes the store paths specified by [*installables*](./bsd.md#installables),
but only if it is safe to do so; that is, when the path is not
reachable from a root of the garbage collector. This means that you
can only delete paths that would also be deleted by `bsd store
gc`. Thus, `bsd store delete` is a more targeted version of `bsd store
gc`.

With the option `--ignore-liveness`, reachability from the roots is
ignored. However, the path still won't be deleted if there are other
paths in the store that refer to it (i.e., depend on it).

)""
