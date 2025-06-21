R""(

# Examples

* Set the `bsdpkgs` flake identifier to a specific branch of Bsdpkgs:

  ```console
  # bsd registry add bsdpkgs github:BasedLinux/bsdpkgs/bsdos-20.03
  ```

* Pin `bsdpkgs` to a specific revision:

  ```console
  # bsd registry add bsdpkgs github:BasedLinux/bsdpkgs/925b70cd964ceaedee26fde9b19cc4c4f081196a
  ```

* Add an entry that redirects a specific branch of `bsdpkgs` to
  another fork:

  ```console
  # bsd registry add bsdpkgs/bsdos-20.03 ~/Dev/bsdpkgs
  ```

* Add `bsdpkgs` pointing to `github:bsdos/bsdpkgs` to your custom flake
  registry:

  ```console
  bsd registry add --registry ./custom-flake-registry.json bsdpkgs github:bsdos/bsdpkgs
  ```

# Description

This command adds an entry to the user registry that maps flake
reference *from-url* to flake reference *to-url*. If an entry for
*from-url* already exists, it is overwritten.

Entries can be removed using [`bsd registry
remove`](./bsd3-registry-remove.md).

)""
