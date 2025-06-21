R""(

# Examples

* Test whether connecting to a remote Bsd store via SSH works:

  ```console
  # bsd store info --store ssh://mac1
  ```

* Test whether a URL is a valid binary cache:

  ```console
  # bsd store info --store https://cache.basedlinux.org
  ```

* Test whether the Bsd daemon is up and running:

  ```console
  # bsd store info --store daemon
  ```

# Description

This command tests whether a particular Bsd store (specified by the
argument `--store` *url*) can be accessed. What this means is
dependent on the type of the store. For instance, for an SSH store it
means that Bsd can connect to the specified machine.

If the command succeeds, Bsd returns a exit code of 0 and does not
print any output.

)""
