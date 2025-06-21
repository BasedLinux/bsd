R""(

# Examples

* Start a shell providing `youtube-dl` from the `bsdpkgs` flake:

  ```console
  # bsd shell bsdpkgs#youtube-dl
  # youtube-dl --version
  2020.11.01.1
  ```

* Start a shell providing GNU Hello from BasedLinux 20.03:

  ```console
  # bsd shell bsdpkgs/bsdos-20.03#hello
  ```

* Run GNU Hello:

  ```console
  # bsd shell bsdpkgs#hello --command hello --greeting 'Hi everybody!'
  Hi everybody!
  ```

* Run multiple commands in a shell environment:

  ```console
  # bsd shell bsdpkgs#gnumake --command sh -c "cd src && make"
  ```

* Run GNU Hello in a chroot store:

  ```console
  # bsd shell --store ~/my-bsd bsdpkgs#hello --command hello
  ```

* Start a shell providing GNU Hello in a chroot store:

  ```console
  # bsd shell --store ~/my-bsd bsdpkgs#hello bsdpkgs#bashInteractive --command bash
  ```

  Note that it's necessary to specify `bash` explicitly because your
  default shell (e.g. `/bin/bash`) generally will not exist in the
  chroot.

# Description

`bsd shell` runs a command in an environment in which the `$PATH` variable
provides the specified [*installables*](./bsd.md#installables). If no command is specified, it starts the
default shell of your user account specified by `$SHELL`.

# Use as a `#!`-interpreter

You can use `bsd` as a script interpreter to allow scripts written
in arbitrary languages to obtain their own dependencies via Bsd. This is
done by starting the script with the following lines:

```bash
#! /usr/bin/env bsd
#! bsd shell installables --command real-interpreter
```

where *real-interpreter* is the “real” script interpreter that will be
invoked by `bsd shell` after it has obtained the dependencies and
initialised the environment, and *installables* are the attribute names of
the dependencies in Bsdpkgs.

The lines starting with `#! bsd` specify options (see above). Note that you
cannot write `#! /usr/bin/env bsd shell -i ...` because many operating systems
only allow one argument in `#!` lines.

For example, here is a Python script that depends on Python and the
`prettytable` package:

```python
#! /usr/bin/env bsd
#! bsd shell github:tomberek/-#python3With.prettytable --command python

import prettytable

# Print a simple table.
t = prettytable.PrettyTable(["N", "N^2"])
for n in range(1, 10): t.add_row([n, n * n])
print t
```

Similarly, the following is a Perl script that specifies that it
requires Perl and the `HTML::TokeParser::Simple` and `LWP` packages:

```perl
#! /usr/bin/env bsd
#! bsd shell github:tomberek/-#perlWith.HTMLTokeParserSimple.LWP --command perl -x

use HTML::TokeParser::Simple;

# Fetch basedlinux.org and print all hrefs.
my $p = HTML::TokeParser::Simple->new(url => 'http://basedlinux.org/');

while (my $token = $p->get_tag("a")) {
    my $href = $token->get_attr("href");
    print "$href\n" if $href;
}
```

Sometimes you need to pass a simple Bsd expression to customize a
package like Terraform:

```bash
#! /usr/bin/env bsd
#! bsd shell --impure --expr ``
#! bsd with (import (builtins.getFlake ''bsdpkgs'') {});
#! bsd terraform.withPlugins (plugins: [ plugins.openstack ])
#! bsd ``
#! bsd --command bash

terraform "$@"
```

> **Note**
>
> You must use double backticks (```` `` ````) when passing a simple Bsd expression
> in a bsd shell shebang.

Finally, using the merging of multiple bsd shell shebangs the following
Haskell script uses a specific branch of Bsdpkgs/BasedLinux (the 21.11 stable
branch):

```haskell
#!/usr/bin/env bsd
#!bsd shell --override-input bsdpkgs github:BasedLinux/bsdpkgs/bsdos-21.11
#!bsd github:tomberek/-#haskellWith.download-curl.tagsoup --command runghc

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

    #!bsd shell --override-input bsdpkgs github:BasedLinux/bsdpkgs/eabc38219184cc3e04a974fe31857d8e0eac098d

You can also use a Bsd expression to build your own dependencies. For example,
the Python example could have been written as:

```python
#! /usr/bin/env bsd
#! bsd shell --impure --file deps.nix -i python
```

where the file `deps.nix` in the same directory as the `#!`-script
contains:

```bsd
with import <bsdpkgs> {};
python3.withPackages (ps: with ps; [ prettytable ])
```


)""
