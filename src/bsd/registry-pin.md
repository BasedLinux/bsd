R""(

# Examples

* Pin `bsdpkgs` to its most recent Git revision:

  ```console
  # bsd registry pin bsdpkgs
  ```

  Afterwards the user registry will have an entry like this:

  ```console
  bsd registry list | grep '^user '
  user   flake:bsdpkgs github:BasedLinux/bsdpkgs/925b70cd964ceaedee26fde9b19cc4c4f081196a
  ```

  and `bsd flake metadata` will say:

  ```console
  # bsd flake metadata bsdpkgs
  Resolved URL:  github:BasedLinux/bsdpkgs/925b70cd964ceaedee26fde9b19cc4c4f081196a
  Locked URL:    github:BasedLinux/bsdpkgs/925b70cd964ceaedee26fde9b19cc4c4f081196a
  …
  ```

* Pin `bsdpkgs` in a custom registry to its most recent Git revision:

  ```console
  # bsd registry pin --registry ./custom-flake-registry.json bsdpkgs
  ```


# Description

This command adds an entry to the user registry that maps flake
reference *url* to the corresponding *locked* flake reference, that
is, a flake reference that specifies an exact revision or content
hash. This ensures that until this registry entry is removed, all uses
of *url* will resolve to exactly the same flake.

Entries can be removed using [`bsd registry
remove`](./bsd3-registry-remove.md).

)""
