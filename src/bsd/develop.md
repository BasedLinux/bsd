R""(

# Examples

* Start a shell with the build environment of the default package of
  the flake in the current directory:

  ```console
  # bsd develop
  ```

  Typical commands to run inside this shell are:

  ```console
  # configurePhase
  # buildPhase
  # installPhase
  ```

  Alternatively, you can run whatever build tools your project uses
  directly, e.g. for a typical Ubsd project:

  ```console
  # ./configure --prefix=$out
  # make
  # make install
  ```

* Run a particular build phase directly:

  ```console
  # bsd develop --unpack
  # bsd develop --configure
  # bsd develop --build
  # bsd develop --check
  # bsd develop --install
  # bsd develop --installcheck
  ```

* Start a shell with the build environment of GNU Hello:

  ```console
  # bsd develop bsdpkgs#hello
  ```

* Record a build environment in a profile:

  ```console
  # bsd develop --profile /tmp/my-build-env bsdpkgs#hello
  ```

* Use a build environment previously recorded in a profile:

  ```console
  # bsd develop /tmp/my-build-env
  ```

* Replace all occurrences of the store path corresponding to
  `glibc.dev` with a writable directory:

  ```console
  # bsd develop --redirect bsdpkgs#glibc.dev ~/my-glibc/outputs/dev
  ```

  Note that this is useful if you're running a `bsd develop` shell for
  `bsdpkgs#glibc` in `~/my-glibc` and want to compile another package
  against it.

* Run a series of script commands:

  ```console
  # bsd develop --command bash -c "mkdir build && cmake .. && make"
  ```

# Description

`bsd develop` starts a `bash` shell that provides an interactive build
environment nearly identical to what Bsd would use to build
[*installable*](./bsd.md#installables). Inside this shell, environment variables and shell
functions are set up so that you can interactively and incrementally
build your package.

Bsd determines the build environment by building a modified version of
the derivation *installable* that just records the environment
initialised by `stdenv` and exits. This build environment can be
recorded into a profile using `--profile`.

The prompt used by the `bash` shell can be customised by setting the
`bash-prompt`, `bash-prompt-prefix`, and `bash-prompt-suffix` settings in
`bsd.conf` or in the flake's `bsdConfig` attribute.

# Flake output attributes

If no flake output attribute is given, `bsd develop` tries the following
flake output attributes:

* `devShells.<system>.default`

* `packages.<system>.default`

If a flake output *name* is given, `bsd develop` tries the following flake
output attributes:

* `devShells.<system>.<name>`

* `packages.<system>.<name>`

* `legacyPackages.<system>.<name>`

)""
