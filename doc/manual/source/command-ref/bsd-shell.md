# Name

`bsd-shell` - start an interactive shell based on a Bsd expression

# Synopsis

`bsd-shell`
  [`--arg` *name* *value*]
  [`--argstr` *name* *value*]
  [{`--attr` | `-A`} *attrPath*]
  [`--command` *cmd*]
  [`--run` *cmd*]
  [`--exclude` *regexp*]
  [`--pure`]
  [`--keep` *name*]
  {{`--packages` | `-p`} {*packages* | *expressions*} … | [*path*]}

# Disambiguation

This man page describes the command `bsd-shell`, which is distinct from `bsd
shell`. For documentation on the latter, run `bsd shell --help` or see `man
bsd3-shell`.

# Description

The command `bsd-shell` will build the dependencies of the specified
derivation, but not the derivation itself. It will then start an
interactive shell in which all environment variables defined by the
derivation *path* have been set to their corresponding values, and the
script `$stdenv/setup` has been sourced. This is useful for reproducing
the environment of a derivation for development.

If *path* is not given, `bsd-shell` defaults to `shell.nix` if it
exists, and `default.nix` otherwise.

If *path* starts with `http://` or `https://`, it is interpreted as the
URL of a tarball that will be downloaded and unpacked to a temporary
location. The tarball must include a single top-level directory
containing at least a file named `default.nix`.

If the derivation defines the variable `shellHook`, it will be run
after `$stdenv/setup` has been sourced. Since this hook is not executed
by regular Bsd builds, it allows you to perform initialisation specific
to `bsd-shell`. For example, the derivation attribute

```bsd
shellHook =
  ''
    echo "Hello shell"
    export SOME_API_TOKEN="$(cat ~/.config/some-app/api-token)"
  '';
```

will cause `bsd-shell` to print `Hello shell` and set the `SOME_API_TOKEN`
environment variable to a user-configured value.

# Options

All options not listed here are passed to `bsd-store
--realise`, except for `--arg` and `--attr` / `-A` which are passed to
`bsd-instantiate`.

- `--command` *cmd*

  In the environment of the derivation, run the shell command *cmd*.
  This command is executed in an interactive shell. (Use `--run` to
  use a non-interactive shell instead.) However, a call to `exit` is
  implicitly added to the command, so the shell will exit after
  running the command. To prevent this, add `return` at the end;
  e.g.  `--command "echo Hello; return"` will print `Hello` and then
  drop you into the interactive shell. This can be useful for doing
  any additional initialisation.

- `--run` *cmd*

  Like `--command`, but executes the command in a non-interactive
  shell. This means (among other things) that if you hit Ctrl-C while
  the command is running, the shell exits.

- `--exclude` *regexp*

  Do not build any dependencies whose store path matches the regular
  expression *regexp*. This option may be specified multiple times.

- `--pure`

  If this flag is specified, the environment is almost entirely
  cleared before the interactive shell is started, so you get an
  environment that more closely corresponds to the “real” Bsd build. A
  few variables, in particular `HOME`, `USER` and `DISPLAY`, are
  retained.  Note that the shell used to run commands is obtained from
  [`NIX_BUILD_SHELL`](#env-NIX_BUILD_SHELL) / `<bsdpkgs>` from
  `NIX_PATH`, and therefore not affected by `--pure`.

- `--packages` / `-p` *packages*…

  Set up an environment in which the specified packages are present.
  The command line arguments are interpreted as attribute names inside
  the Bsd Packages collection. Thus, `bsd-shell --packages libjpeg openjdk`
  will start a shell in which the packages denoted by the attribute
  names `libjpeg` and `openjdk` are present.

- `-i` *interpreter*

  The chained script interpreter to be invoked by `bsd-shell`. Only
  applicable in `#!`-scripts (described below).

- `--keep` *name*

  When a `--pure` shell is started, keep the listed environment
  variables.

{{#include ./opt-common.md}}

# Environment variables

- <span id="env-NIX_BUILD_SHELL">[`NIX_BUILD_SHELL`](#env-NIX_BUILD_SHELL)</span>

  Shell used to start the interactive environment.
  Defaults to the `bash` from `bashInteractive` found in `<bsdpkgs>`, falling back to the `bash` found in `PATH` if not found.

  > **Note**
  >
  > The shell obtained using this method may not necessarily be the same as any shells requested in *path*.

  <!-- -->

  > **Example
  >
  >  Despite `--pure`, this invocation will not result in a fully reproducible shell environment:
  >
  > ```bsd
  > #!/usr/bin/env -S bsd-shell --pure
  > let
  >   pkgs = import (fetchTarball "https://github.com/BasedLinux/bsdpkgs/archive/854fdc68881791812eddd33b2fed94b954979a8e.tar.gz") {};
  > in
  > pkgs.mkShell {
  >   buildInputs = pkgs.bashInteractive;
  > }
  > ```

{{#include ./env-common.md}}

# Examples

To build the dependencies of the package Pan, and start an interactive
shell in which to build it:

```console
$ bsd-shell '<bsdpkgs>' --attr pan
[bsd-shell]$ eval ${unpackPhase:-unpackPhase}
[bsd-shell]$ cd $sourceRoot
[bsd-shell]$ eval ${patchPhase:-patchPhase}
[bsd-shell]$ eval ${configurePhase:-configurePhase}
[bsd-shell]$ eval ${buildPhase:-buildPhase}
[bsd-shell]$ ./pan/gui/pan
```

The reason we use form `eval ${configurePhase:-configurePhase}` here is because
those packages that override these phases do so by exporting the overridden
values in the environment variable of the same name.
Here bash is being told to either evaluate the contents of 'configurePhase',
if it exists as a variable, otherwise evaluate the configurePhase function.

To clear the environment first, and do some additional automatic
initialisation of the interactive shell:

```console
$ bsd-shell '<bsdpkgs>' --attr pan --pure \
    --command 'export NIX_DEBUG=1; export NIX_CORES=8; return'
```

Bsd expressions can also be given on the command line using the `-E` and
`-p` flags. For instance, the following starts a shell containing the
packages `sqlite` and `libX11`:

```console
$ bsd-shell --expr 'with import <bsdpkgs> { }; runCommand "dummy" { buildInputs = [ sqlite xorg.libX11 ]; } ""'
```

A shorter way to do the same is:

```console
$ bsd-shell --packages sqlite xorg.libX11
[bsd-shell]$ echo $NIX_LDFLAGS
… -L/bsd/store/j1zg5v…-sqlite-3.8.0.2/lib -L/bsd/store/0gmcz9…-libX11-1.6.1/lib …
```

Note that `-p` accepts multiple full bsd expressions that are valid in
the `buildInputs = [ ... ]` shown above, not only package names. So the
following is also legal:

```console
$ bsd-shell --packages sqlite 'git.override { withManual = false; }'
```

The `-p` flag looks up Bsdpkgs in the Bsd search path. You can override
it by passing `-I` or setting `NIX_PATH`. For example, the following
gives you a shell containing the Pan package from a specific revision of
Bsdpkgs:

```console
$ bsd-shell --packages pan -I bsdpkgs=https://github.com/BasedLinux/bsdpkgs/archive/8a3eea054838b55aca962c3fbde9c83c102b8bf2.tar.gz

[bsd-shell:~]$ pan --version
Pan 0.139
```

# Use as a `#!`-interpreter

You can use `bsd-shell` as a script interpreter to allow scripts written
in arbitrary languages to obtain their own dependencies via Bsd. This is
done by starting the script with the following lines:

```bash
#! /usr/bin/env bsd-shell
#! bsd-shell -i real-interpreter --packages packages
```

where *real-interpreter* is the “real” script interpreter that will be
invoked by `bsd-shell` after it has obtained the dependencies and
initialised the environment, and *packages* are the attribute names of
the dependencies in Bsdpkgs.

The lines starting with `#! bsd-shell` specify `bsd-shell` options (see
above). Note that you cannot write `#! /usr/bin/env bsd-shell -i ...`
because many operating systems only allow one argument in `#!` lines.

For example, here is a Python script that depends on Python and the
`prettytable` package:

```python
#! /usr/bin/env bsd-shell
#! bsd-shell -i python3 --packages python3 python3Packages.prettytable

import prettytable

# Print a simple table.
t = prettytable.PrettyTable(["N", "N^2"])
for n in range(1, 10): t.add_row([n, n * n])
print(t)
```

Similarly, the following is a Perl script that specifies that it
requires Perl and the `HTML::TokeParser::Simple`, `LWP` and
`LWP::Protocol::Https` packages:

```perl
#! /usr/bin/env bsd-shell
#! bsd-shell -i perl 
#! bsd-shell --packages perl 
#! bsd-shell --packages perlPackages.HTMLTokeParserSimple 
#! bsd-shell --packages perlPackages.LWP
#! bsd-shell --packages perlPackages.LWPProtocolHttps

use HTML::TokeParser::Simple;

# Fetch basedlinux.org and print all hrefs.
my $p = HTML::TokeParser::Simple->new(url => 'https://basedlinux.org/');

while (my $token = $p->get_tag("a")) {
    my $href = $token->get_attr("href");
    print "$href\n" if $href;
}
```

Sometimes you need to pass a simple Bsd expression to customize a
package like Terraform:

```bash
#! /usr/bin/env bsd-shell
#! bsd-shell -i bash --packages 'terraform.withPlugins (plugins: [ plugins.openstack ])'

terraform apply
```

> **Note**
>
> You must use single or double quotes (`'`, `"`) when passing a simple Bsd expression
> in a bsd-shell shebang.

Finally, using the merging of multiple bsd-shell shebangs the following
Haskell script uses a specific branch of Bsdpkgs/BasedLinux (the 20.03 stable
branch):

```haskell
#! /usr/bin/env bsd-shell
#! bsd-shell -i runghc --packages 'haskellPackages.ghcWithPackages (ps: [ps.download-curl ps.tagsoup])'
#! bsd-shell -I bsdpkgs=https://github.com/BasedLinux/bsdpkgs/archive/bsdos-20.03.tar.gz

import Network.Curl.Download
import Text.HTML.TagSoup
import Data.Either
import Data.ByteString.Char8 (unpack)

-- Fetch basedlinux.org and print all hrefs.
main = do
  resp <- openURI "https://basedlinux.org/"
  let tags = filter (isTagOpenName "a") $ parseTags $ unpack $ fromRight undefined resp
  let tags' = map (fromAttrib "href") tags
  mapM_ putStrLn $ filter (/= "") tags'
```

If you want to be even more precise, you can specify a specific revision
of Bsdpkgs:

    #! bsd-shell -I bsdpkgs=https://github.com/BasedLinux/bsdpkgs/archive/0672315759b3e15e2121365f067c1c8c56bb4722.tar.gz

The examples above all used `-p` to get dependencies from Bsdpkgs. You
can also use a Bsd expression to build your own dependencies. For
example, the Python example could have been written as:

```python
#! /usr/bin/env bsd-shell
#! bsd-shell deps.nix -i python
```

where the file `deps.nix` in the same directory as the `#!`-script
contains:

```bsd
with import <bsdpkgs> {};

runCommand "dummy" { buildInputs = [ python3 python3Packages.prettytable ]; } ""
```

The script's file name is passed as the first argument to the interpreter specified by the `-i` flag.

Aside from the very first line, which is a directive to the operating system, the additional `#! bsd-shell` lines do not need to be at the beginning of the file.
This allows wrapping them in block comments for languages where `#` does not start a comment, such as ECMAScript, Erlang, PHP, or Ruby.
