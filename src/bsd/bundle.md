R""(

# Examples

* Bundle Hello:

  ```console
  # bsd bundle bsdpkgs#hello
  # ./hello
  Hello, world!
  ```

* Bundle a specific version of Bsd:

  ```console
  # bsd bundle github:BasedLinux/bsd/e3ddffb27e5fc37a209cfd843c6f7f6a9460a8ec
  # ./bsd --version
  bsd (Bsd) 2.4pre20201215_e3ddffb
  ```

* Bundle a Hello using a specific bundler:

  ```console
  # bsd bundle --bundler github:BasedLinux/bundlers#toDockerImage bsdpkgs#hello
  # docker load < hello-2.10.tar.gz
  # docker run hello-2.10:latest hello
  Hello, world!
  ```

# Description

`bsd bundle`, by default, packs the closure of the [*installable*](./bsd.md#installables) into a single
self-extracting executable. See the [`bundlers`
homepage](https://github.com/BasedLinux/bundlers) for more details.

> **Note**
>
> This command only works on Linux.

# Flake output attributes

If no flake output attribute is given, `bsd bundle` tries the following
flake output attributes:

* `bundlers.<system>.default`

If an attribute *name* is given, `bsd bundle` tries the following flake
output attributes:

* `bundlers.<system>.<name>`

# Bundlers

A bundler is specified by a flake output attribute named
`bundlers.<system>.<name>`. It looks like this:

```bsd
bundlers.x86_64-linux = rec {
  identity = drv: drv;

  blender_2_79 = drv: self.packages.x86_64-linux.blender_2_79;

  default = identity;
};
```

A bundler must be a function that accepts an arbitrary value (typically a
derivation or app definition) and returns a derivation.

)""
