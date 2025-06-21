R""(

# Examples

* Get the build log of GNU Hello:

  ```console
  # bsd log bsdpkgs#hello
  ```

* Get the build log of a specific store path:

  ```console
  # bsd log /bsd/store/lmngj4wcm9rkv3w4dfhzhcyij3195hiq-thunderbird-52.2.1
  ```

* Get a build log from a specific binary cache:

  ```console
  # bsd log --store https://cache.basedlinux.org bsdpkgs#hello
  ```

# Description

This command prints the log of a previous build of the [*installable*](./bsd.md#installables) on standard output.

Bsd looks for build logs in two places:

* In the directory `/bsd/var/log/bsd/drvs`, which contains logs for
  locally built derivations.

* In the binary caches listed in the `substituters` setting. Logs
  should be named `<cache>/log/<base-name-of-store-path>`, where
  `store-path` is a derivation,
  e.g. `https://cache.basedlinux.org/log/dvmig8jgrdapvbyxb1rprckdmdqx08kv-hello-2.10.drv`.
  For non-derivation store paths, Bsd will first try to determine the
  deriver by fetching the `.narinfo` file for this store path.

)""
