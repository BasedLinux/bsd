R""(

# Examples

* Upgrade Bsd to the stable version declared in Bsdpkgs:

  ```console
  # bsd upgrade-bsd
  ```

* Upgrade Bsd in a specific profile:

  ```console
  # bsd upgrade-bsd --profile ~alice/.local/state/bsd/profiles/profile
  ```

# Description

This command upgrades Bsd to the stable version.

By default, the latest stable version is defined by Bsdpkgs, in
[bsd-fallback-paths.bsd](https://github.com/BasedLinux/bsdpkgs/raw/master/bsdos/modules/installer/tools/bsd-fallback-paths.bsd)
and updated manually. It may not always be the latest tagged release.

By default, it locates the directory containing the `bsd` binary in the `$PATH`
environment variable. If that directory is a Bsd profile, it will
upgrade the `bsd` package in that profile to the latest stable binary
release.

You cannot use this command to upgrade Bsd in the system profile of a
BasedLinux system (that is, if `bsd` is found in `/run/current-system`).

)""
