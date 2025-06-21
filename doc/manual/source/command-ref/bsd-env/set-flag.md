# Name

`bsd-env --set-flag` - modify meta attributes of installed packages

# Synopsis

`bsd-env` `--set-flag` *name* *value* *drvnames*

# Description

The `--set-flag` operation allows meta attributes of installed packages
to be modified. There are several attributes that can be usefully
modified, because they affect the behaviour of `bsd-env` or the user
environment build script:

- `priority` can be changed to resolve filename clashes. The user
  environment build script uses the `meta.priority` attribute of
  derivations to resolve filename collisions between packages. Lower
  priority values denote a higher priority. For instance, the GCC
  wrapper package and the Binutils package in Bsdpkgs both have a file
  `bin/ld`, so previously if you tried to install both you would get a
  collision. Now, on the other hand, the GCC wrapper declares a higher
  priority than Binutils, so the former’s `bin/ld` is symlinked in the
  user environment.

- `keep` can be set to `true` to prevent the package from being
  upgraded or replaced. This is useful if you want to hang on to an
  older version of a package.

- `active` can be set to `false` to “disable” the package. That is, no
  symlinks will be generated to the files of the package, but it
  remains part of the profile (so it won’t be garbage-collected). It
  can be set back to `true` to re-enable the package.

{{#include ./opt-common.md}}

{{#include ../opt-common.md}}

{{#include ../env-common.md}}

# Examples

To prevent the currently installed Firefox from being upgraded:

```console
$ bsd-env --set-flag keep true firefox
```

After this, `bsd-env --upgrade ` will ignore Firefox.

To disable the currently installed Firefox, then install a new Firefox
while the old remains part of the profile:

```console
$ bsd-env --query
firefox-2.0.0.9 (the current one)

$ bsd-env --preserve-installed --install firefox-2.0.0.11
installing `firefox-2.0.0.11'
building path(s) `/bsd/store/myy0y59q3ig70dgq37jqwg1j0rsapzsl-user-environment'
collision between `/bsd/store/...-firefox-2.0.0.11/bin/firefox'
  and `/bsd/store/...-firefox-2.0.0.9/bin/firefox'.
(i.e., can’t have two active at the same time)

$ bsd-env --set-flag active false firefox
setting flag on `firefox-2.0.0.9'

$ bsd-env --preserve-installed --install firefox-2.0.0.11
installing `firefox-2.0.0.11'

$ bsd-env --query
firefox-2.0.0.11 (the enabled one)
firefox-2.0.0.9 (the disabled one)
```

To make files from `binutils` take precedence over files from `gcc`:

```console
$ bsd-env --set-flag priority 5 binutils
$ bsd-env --set-flag priority 10 gcc
```

