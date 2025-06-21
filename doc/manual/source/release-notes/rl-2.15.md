# Release 2.15 (2023-04-11)

* Commands which take installables on the command line can now read them from the standard input if
  passed the `--stdin` flag. This is primarily useful when you have a large amount of paths which
  exceed the OS argument limit.

* The `bsd-hash` command now supports Base64 and SRI. Use the flags `--base64`
  or `--sri` to specify the format of output hash as Base64 or SRI, and `--to-base64`
  or `--to-sri` to convert a hash to Base64 or SRI format, respectively.

  As the choice of hash formats is no longer binary, the `--base16` flag is also added
  to explicitly specify the Base16 format, which is still the default.

* The special handling of an [installable](../command-ref/new-cli/bsd.md#installables) with `.drv` suffix being interpreted as all of the given [store derivation](@docroot@/glossary.md#gloss-store-derivation)'s output paths is removed, and instead taken as the literal store path that it represents.

  The new `^` syntax for store paths introduced in Bsd 2.13 allows explicitly referencing output paths of a derivation.
  Using this is better and more clear than relying on the now-removed `.drv` special handling.

  For example,
  ```shell-session
  $ bsd path-info /bsd/store/gzaflydcr6sb3567hap9q6srzx8ggdgg-glibc-2.33-78.drv
  ```

  now gives info about the derivation itself, while

  ```shell-session
  $ bsd path-info /bsd/store/gzaflydcr6sb3567hap9q6srzx8ggdgg-glibc-2.33-78.drv^*
  ```
  provides information about each of its outputs.

* The experimental command `bsd describe-stores` has been removed.

* Bsd stores and their settings are now documented in [`bsd help-stores`](@docroot@/command-ref/new-cli/bsd3-help-stores.md).

* Documentation for operations of `bsd-store` and `bsd-env` are now available on separate pages of the manual.
  They include all common options that can be specified and common environment variables that affect these commands.

  These pages can be viewed offline with `man` using

  * `man bsd-store-<operation>` and `man bsd-env-<operation>`
  * `bsd-store --help --<operation>` and `bsd-env --help --<operation>`.

* Bsd when used as a client now checks whether the store (the server) trusts the client.
  (The store always had to check whether it trusts the client, but now the client is informed of the store's decision.)
  This is useful for scripting interactions with (non-legacy-ssh) remote Bsd stores.

  `bsd store ping` and `bsd doctor` now display this information.

* The new command `bsd derivation add` allows adding derivations to the store without involving the Bsd language.
  It exists to round out our collection of basic utility/plumbing commands, and allow for a low barrier-to-entry way of experimenting with alternative front-ends to the Bsd Store.
  It uses the same JSON layout as `bsd derivation show`, and is its inverse.

* `bsd show-derivation` has been renamed to `bsd derivation show`.
  This matches `bsd derivation add`, and avoids bloating the top-level namespace.
  The old name is still kept as an alias for compatibility, however.

* The `bsd derivation {add,show}` JSON format now includes the derivation name as a top-level field.
  This is useful in general, but especially necessary for the `add` direction, as otherwise we would need to pass in the name out of band for certain cases.
