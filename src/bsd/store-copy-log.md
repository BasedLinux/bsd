R""(

# Examples

* To copy the build log of the `hello` package from
  https://cache.basedlinux.org to the local store:

  ```console
  # bsd store copy-log --from https://cache.basedlinux.org --eval-store auto bsdpkgs#hello
  ```

  You can verify that the log is available locally:

  ```console
  # bsd log --substituters '' bsdpkgs#hello
  ```

  (The flag `--substituters ''` avoids querying
  `https://cache.basedlinux.org` for the log.)

* To copy the log for a specific [store derivation] via SSH:

  [store derivation]: @docroot@/glossary.md#gloss-store-derivation

  ```console
  # bsd store copy-log --to ssh-ng://machine /bsd/store/ilgm50plpmcgjhcp33z6n4qbnpqfhxym-glibc-2.33-59.drv
  ```

# Description

`bsd store copy-log` copies build logs between two Bsd stores. The
source store is specified using `--from` and the destination using
`--to`. If one of these is omitted, it defaults to the local store.

)""
