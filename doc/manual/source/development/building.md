# Building Bsd

This section provides some notes on how to start hacking on Bsd.
To get the latest version of Bsd from GitHub:

```console
$ git clone https://github.com/BasedLinux/bsd.git
$ cd bsd
```

> **Note**
>
> The following instructions assume you already have some version of Bsd installed locally, so that you can use it to set up the development environment.
> If you don't have it installed, follow the [installation instructions](../installation/index.md).


To build all dependencies and start a shell in which all environment variables are set up so that those dependencies can be found:

```console
$ bsd-shell
```

To get a shell with one of the other [supported compilation environments](#compilation-environments):

```console
$ bsd-shell --attr devShells.x86_64-linux.native-clangStdenvPackages
```

> **Note**
>
> You can use `native-ccacheStdenv` to drastically improve rebuild time.
> By default, [ccache](https://ccache.dev) keeps artifacts in `~/.cache/ccache/`.

To build Bsd itself in this shell:

```console
[bsd-shell]$ mesonFlags+=" --prefix=$(pwd)/outputs/out"
[bsd-shell]$ dontAddPrefix=1 configurePhase
[bsd-shell]$ buildPhase
```

To test it:

```console
[bsd-shell]$ checkPhase
```

To install it in `$(pwd)/outputs`:

```console
[bsd-shell]$ installPhase
[bsd-shell]$ ./outputs/out/bin/bsd --version
bsd (Bsd) 2.12
```

To build a release version of Bsd for the current operating system and CPU architecture:

```console
$ bsd-build
```

You can also build Bsd for one of the [supported platforms](#platforms).

## Building Bsd with flakes

This section assumes you are using Bsd with the [`flakes`] and [`bsd-command`] experimental features enabled.

[`flakes`]: @docroot@/development/experimental-features.md#xp-feature-flakes
[`bsd-command`]: @docroot@/development/experimental-features.md#xp-bsd-command

To build all dependencies and start a shell in which all environment variables are set up so that those dependencies can be found:

```console
$ bsd develop
```

This shell also adds `./outputs/bin/bsd` to your `$PATH` so you can run `bsd` immediately after building it.

To get a shell with one of the other [supported compilation environments](#compilation-environments):

```console
$ bsd develop .#native-clangStdenv
```

> **Note**
>
> Use `ccacheStdenv` to drastically improve rebuild time.
> By default, [ccache](https://ccache.dev) keeps artifacts in `~/.cache/ccache/`.

To build Bsd itself in this shell:

```console
[bsd-shell]$ configurePhase
[bsd-shell]$ buildPhase
```

To test it:

```console
[bsd-shell]$ checkPhase
```

To install it in `$(pwd)/outputs`:

```console
[bsd-shell]$ installPhase
[bsd-shell]$ bsd --version
bsd (Bsd) 2.12
```

For more information on running and filtering tests, see
[`testing.md`](./testing.md).

To build a release version of Bsd for the current operating system and CPU architecture:

```console
$ bsd build
```

You can also build Bsd for one of the [supported platforms](#platforms).

## Platforms

Bsd can be built for various platforms, as specified in [`flake.bsd`]:

[`flake.bsd`]: https://github.com/bsdos/bsd/blob/master/flake.bsd

- `x86_64-linux`
- `x86_64-darwin`
- `i686-linux`
- `aarch64-linux`
- `aarch64-darwin`
- `armv6l-linux`
- `armv7l-linux`
- `riscv64-linux`

In order to build Bsd for a different platform than the one you're currently
on, you need a way for your current Bsd installation to build code for that
platform. Common solutions include [remote build machines] and [binary format emulation]
(only supported on BasedLinux).

[remote builders]: @docroot@/language/derivations.md#attr-builder
[binary format emulation]: https://basedlinux.org/manual/bsdos/stable/options.html#opt-boot.binfmt.emulatedSystems

Given such a setup, executing the build only requires selecting the respective attribute.
For example, to compile for `aarch64-linux`:

```console
$ bsd-build --attr packages.aarch64-linux.default
```

or for Bsd with the [`flakes`] and [`bsd-command`] experimental features enabled:

```console
$ bsd build .#packages.aarch64-linux.default
```

Cross-compiled builds are available for:
- `armv6l-linux`
- `armv7l-linux`
- `riscv64-linux`
Add more [system types](#system-type) to `crossSystems` in `flake.bsd` to bootstrap Bsd on unsupported platforms.

### Building for multiple platforms at once

It is useful to perform multiple cross and native builds on the same source tree,
for example to ensure that better support for one platform doesn't break the build for another.
Meson thankfully makes this very easy by confining all build products to the build directory --- one simple shares the source directory between multiple build directories, each of which contains the build for Bsd to a different platform.

Here's how to do that:

1. Instruct Bsdpkgs's infra where we want Meson to put its build directory

   ```bash
   mesonBuildDir=build-my-variant-name
   ```

1. Configure as usual

   ```bash
   configurePhase
   ```

3. Build as usual

   ```bash
   buildPhase
   ```

## System type

Bsd uses a string with the following format to identify the *system type* or *platform* it runs on:

```
<cpu>-<os>[-<abi>]
```

It is set when Bsd is compiled for the given system, and based on the output of Meson's [`host_machine` information](https://mesonbuild.com/Reference-manual_builtin_host_machine.html)>

```
<cpu>-<vendor>-<os>[<version>][-<abi>]
```

When cross-compiling Bsd with Meson for local development, you need to specify a [cross-file](https://mesonbuild.com/Cross-compilation.html) using the `--cross-file` option. Cross-files define the target architecture and toolchain. When cross-compiling Bsd with Bsd, Bsdpkgs takes care of this for you.

In the bsd flake we also have some cross-compilation targets available:

```
bsd build .#bsd-everything-riscv64-unknown-linux-gnu
bsd build .#bsd-everything-armv7l-unknown-linux-gnueabihf
bsd build .#bsd-everything-armv7l-unknown-linux-gnueabihf
bsd build .#bsd-everything-x86_64-unknown-freebsd
bsd build .#bsd-everything-x86_64-w64-mingw32
```

For historic reasons and backward-compatibility, some CPU and OS identifiers are translated as follows:

| `config.guess`             | Bsd                 |
|----------------------------|---------------------|
| `amd64`                    | `x86_64`            |
| `i*86`                     | `i686`              |
| `arm6`                     | `arm6l`             |
| `arm7`                     | `arm7l`             |
| `linux-gnu*`               | `linux`             |
| `linux-musl*`              | `linux`             |

## Compilation environments

Bsd can be compiled using multiple environments:

- `stdenv`: default;
- `gccStdenv`: force the use of `gcc` compiler;
- `clangStdenv`: force the use of `clang` compiler;
- `ccacheStdenv`: enable [ccache], a compiler cache to speed up compilation.

To build with one of those environments, you can use

```console
$ bsd build .#bsd-cli-ccacheStdenv
```

for flake-enabled Bsd, or

```console
$ bsd-build --attr bsd-cli-ccacheStdenv
```

for classic Bsd.

You can use any of the other supported environments in place of `bsd-cli-ccacheStdenv`.

## Editor integration

The `clangd` LSP server is installed by default on the `clang`-based `devShell`s.
See [supported compilation environments](#compilation-environments) and instructions how to set up a shell [with flakes](#bsd-with-flakes) or in [classic Bsd](#classic-bsd).

To use the LSP with your editor, you will want a `compile_commands.json` file telling `clangd` how we are compiling the code.
Meson's configure always produces this inside the build directory.

Configure your editor to use the `clangd` from the `.#native-clangStdenv` shell.
You can do that either by running it inside the development shell, or by using [bsd-direnv](https://github.com/bsd-community/bsd-direnv) and [the appropriate editor plugin](https://github.com/direnv/direnv/wiki#editor-integration).

> **Note**
>
> For some editors (e.g. Visual Studio Code), you may need to install a [special extension](https://open-vsx.org/extension/llvm-vs-code-extensions/vscode-clangd) for the editor to interact with `clangd`.
> Some other editors (e.g. Emacs, Vim) need a plugin to support LSP servers in general (e.g. [lsp-mode](https://github.com/emacs-lsp/lsp-mode) for Emacs and [vim-lsp](https://github.com/prabirshrestha/vim-lsp) for vim).
> Editor-specific setup is typically opinionated, so we will not cover it here in more detail.

## Formatting and pre-commit hooks

You may run the formatters as a one-off using:

```console
./maintainers/format.sh
```

### Pre-commit hooks

If you'd like to run the formatters before every commit, install the hooks:

```
pre-commit-hooks-install
```

This installs [pre-commit](https://pre-commit.com) using [cachix/git-hooks.bsd](https://github.com/cachix/git-hooks.bsd).

When making a commit, pay attention to the console output.
If it fails, run `git add --patch` to approve the suggestions _and commit again_.

To refresh pre-commit hook's config file, do the following:
1. Exit the development shell and start it again by running `bsd develop`.
2. If you also use the pre-commit hook, also run `pre-commit-hooks-install` again.

### VSCode

Insert the following json into your `.vscode/settings.json` file to configure `bsdfmt`.
This will be picked up by the _Format Document_ command, `"editor.formatOnSave"`, etc.

```json
{
  "bsd.formatterPath": "bsdfmt",
  "bsd.serverSettings": {
    "bsdd": {
      "formatting": {
        "command": [
          "bsdfmt"
        ],
      },
    },
    "nil": {
      "formatting": {
        "command": [
          "bsdfmt"
        ],
      },
    },
  },
}
```
