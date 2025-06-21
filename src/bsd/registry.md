R""(

# Description

`bsd registry` provides subcommands for managing *flake
registries*. Flake registries are a convenience feature that allows
you to refer to flakes using symbolic identifiers such as `bsdpkgs`,
rather than full URLs such as `git://github.com/BasedLinux/bsdpkgs`. You
can use these identifiers on the command line (e.g. when you do `bsd
run bsdpkgs#hello`) or in flake input specifications in `flake.bsd`
files. The latter are automatically resolved to full URLs and recorded
in the flake's `flake.lock` file.

In addition, the flake registry allows you to redirect arbitrary flake
references (e.g. `github:BasedLinux/patchelf`) to another location, such as
a local fork.

There are multiple registries. These are, in order from lowest to
highest precedence:

* The global registry, which is a file downloaded from the URL
  specified by the setting `flake-registry`. It is cached locally and
  updated automatically when it's older than `tarball-ttl`
  seconds. The default global registry is kept in [a GitHub
  repository](https://github.com/BasedLinux/flake-registry).

* The system registry, which is shared by all users. The default
  location is `/etc/bsd/registry.json`. On BasedLinux, the system registry
  can be specified using the BasedLinux option `bsd.registry`.

* The user registry `~/.config/bsd/registry.json`. This registry can
  be modified by commands such as `bsd registry pin`.

* Overrides specified on the command line using the option
  `--override-flake`.

Note that the system and user registries are not used to resolve flake references in `flake.bsd`. They are only used to resolve flake references on the command line.

# Registry format

A registry is a JSON file with the following format:

```json
{
  "version": 2,
  "flakes": [
    {
      "from": {
        "type": "indirect",
        "id": "bsdpkgs"
      },
      "to": {
        "type": "github",
        "owner": "BasedLinux",
        "repo": "bsdpkgs"
      }
    },
    ...
  ]
}
```

That is, it contains a list of objects with attributes `from` and
`to`, both of which contain a flake reference in attribute
representation. (For example, `{"type": "indirect", "id": "bsdpkgs"}`
is the attribute representation of `bsdpkgs`, while `{"type":
"github", "owner": "BasedLinux", "repo": "bsdpkgs"}` is the attribute
representation of `github:BasedLinux/bsdpkgs`.)

Given some flake reference *R*, a registry entry is used if its
`from` flake reference *matches* *R*. *R* is then replaced by the
*unification* of the `to` flake reference with *R*.

# Matching

The `from` flake reference in a registry entry *matches* some flake
reference *R* if the attributes in `from` are the same as the
attributes in `R`. For example:

* `bsdpkgs` matches with `bsdpkgs`.

* `bsdpkgs` matches with `bsdpkgs/bsdos-20.09`.

* `bsdpkgs/bsdos-20.09` does not match with `bsdpkgs`.

* `bsdpkgs` does not match with `git://github.com/BasedLinux/patchelf`.

# Unification

The `to` flake reference in a registry entry is *unified* with some flake
reference *R* by taking `to` and applying the `rev` and `ref`
attributes from *R*, if specified. For example:

* `github:BasedLinux/bsdpkgs` unified with `bsdpkgs` produces `github:BasedLinux/bsdpkgs`.

* `github:BasedLinux/bsdpkgs` unified with `bsdpkgs/bsdos-20.09` produces `github:BasedLinux/bsdpkgs/bsdos-20.09`.

* `github:BasedLinux/bsdpkgs/master` unified with `bsdpkgs/bsdos-20.09` produces `github:BasedLinux/bsdpkgs/bsdos-20.09`.

)""
