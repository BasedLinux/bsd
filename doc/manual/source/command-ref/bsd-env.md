# Name

`bsd-env` - manipulate or query Bsd user environments

# Synopsis

`bsd-env` *operation* [*options*] [*arguments…*]
  [`--option` *name* *value*]
  [`--arg` *name* *value*]
  [`--argstr` *name* *value*]
  [{`--file` | `-f`} *path*]
  [{`--profile` | `-p`} *path*]
  [`--system-filter` *system*]
  [`--dry-run`]

# Description

The command `bsd-env` is used to manipulate Bsd user environments. User
environments are sets of software packages available to a user at some
point in time. In other words, they are a synthesised view of the
programs available in the Bsd store. There may be many user
environments: different users can have different environments, and
individual users can switch between different environments.

`bsd-env` takes exactly one *operation* flag which indicates the
subcommand to be performed. The following operations are available:

- [`--install`](./bsd-env/install.md)
- [`--upgrade`](./bsd-env/upgrade.md)
- [`--uninstall`](./bsd-env/uninstall.md)
- [`--set`](./bsd-env/set.md)
- [`--set-flag`](./bsd-env/set-flag.md)
- [`--query`](./bsd-env/query.md)
- [`--switch-profile`](./bsd-env/switch-profile.md)
- [`--list-generations`](./bsd-env/list-generations.md)
- [`--delete-generations`](./bsd-env/delete-generations.md)
- [`--switch-generation`](./bsd-env/switch-generation.md)
- [`--rollback`](./bsd-env/rollback.md)

These pages can be viewed offline:

- `man bsd-env-<operation>`.

  Example: `man bsd-env-install`

- `bsd-env --help --<operation>`

  Example: `bsd-env --help --install`

# Package sources

`bsd-env` can obtain packages from multiple sources:

- An attribute set of derivations from:
  - The [default Bsd expression](@docroot@/command-ref/files/default-bsd-expression.md) (by default)
  - A Bsd file, specified via `--file`
  - A [profile](@docroot@/command-ref/files/profiles.md), specified via `--from-profile`
  - A Bsd expression that is a function which takes default expression as argument, specified via `--from-expression`
- A [store path](@docroot@/store/store-path.md)

# Selectors

Several operations, such as [`bsd-env --query`](./bsd-env/query.md) and [`bsd-env --install`](./bsd-env/install.md), take a list of *arguments* that specify the packages on which to operate.

Packages are identified based on a `name` part and a `version` part of a [symbolic derivation name](@docroot@/language/derivations.md#attr-name):

- `name`: Everything up to but not including the first dash (`-`) that is *not* followed by a letter.
- `version`: The rest, excluding the separating dash.

> **Example**
>
> `bsd-env` parses the symbolic derivation name `apache-httpd-2.0.48` as:
>
> ```json
> {
>   "name": "apache-httpd",
>   "version": "2.0.48"
> }
> ```

> **Example**
>
> `bsd-env` parses the symbolic derivation name `firefox.*` as:
>
> ```json
> {
>   "name": "firefox.*",
>   "version": ""
> }
> ```

The `name` parts of the *arguments* to `bsd-env` are treated as extended regular expressions and matched against the `name` parts of derivation names in the package source.
The match is case-sensitive.
The regular expression can optionally be followed by a dash (`-`) and a version number; if omitted, any version of the package will match.
For details on regular expressions, see [**regex**(7)](https://linux.die.net/man/7/regex).

> **Example**
>
> Common patterns for finding package names with `bsd-env`:
>
> - `firefox`
>
>   Matches the package name `firefox` and any version.
>
> - `firefox-32.0`
>
>   Matches the package name `firefox` and version `32.0`.
>
> - `gtk\\+`
>
>   Matches the package name `gtk+`.
>   The `+` character must be escaped using a backslash (`\`) to prevent it from being interpreted as a quantifier, and the backslash must be escaped in turn with another backslash to ensure that the shell passes it on.
>
> - `.\*`
>
>   Matches any package name.
>   This is the default for most commands.
>
> - `'.*zip.*'`
>
>   Matches any package name containing the string `zip`.
>   Note the dots: `'*zip*'` does not work, because in a regular expression, the character `*` is interpreted as a quantifier.
>
> - `'.*(firefox|chromium).*'`
>
>   Matches any package name containing the strings `firefox` or `chromium`.

# Files

`bsd-env` operates on the following files.

{{#include ./files/default-bsd-expression.md}}

{{#include ./files/profiles.md}}
