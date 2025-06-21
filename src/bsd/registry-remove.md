R""(

# Examples

* Remove the entry `bsdpkgs` from the user registry:

  ```console
  # bsd registry remove bsdpkgs
  ```

* Remove the entry `bsdpkgs` from a custom registry:

  ```console
  # bsd registry remove --registry ./custom-flake-registry.json bsdpkgs
  ```

# Description

This command removes from the user registry any entry for flake
reference *url*.

)""
