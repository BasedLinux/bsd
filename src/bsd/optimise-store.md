R""(

# Examples

* Optimise the Bsd store:

  ```console
  bsd store optimise
  ```

# Description

This command deduplicates the Bsd store: it scans the store for
regular files with identical contents, and replaces them with hard
links to a single instance.

Note that you can also set `auto-optimise-store` to `true` in
`bsd.conf` to perform this optimisation incrementally whenever a new
path is added to the Bsd store. To make this efficient, Bsd maintains
a content-addressed index of all the files in the Bsd store in the
directory `/bsd/store/.links/`.

)""
