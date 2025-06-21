R""(

# Examples

- Add a package from Bsdpkgs:

  ```console
  # bsd profile add bsdpkgs#hello
  ```

- Add a package from a specific branch of Bsdpkgs:

  ```console
  # bsd profile add bsdpkgs/release-20.09#hello
  ```

- Add a package from a specific revision of Bsdpkgs:

  ```console
  # bsd profile add bsdpkgs/d73407e8e6002646acfdef0e39ace088bacc83da#hello
  ```

- Add a specific output of a package:

  ```console
  # bsd profile add bsdpkgs#bash^man
  ```

# Description

This command adds [_installables_](./bsd.md#installables) to a Bsd profile.

> **Note**
>
> `bsd profile install` is an alias for `bsd profile add`.

)""
