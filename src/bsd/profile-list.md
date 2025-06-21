R""(

# Examples

* Show what packages are installed in the default profile:

  ```console
  # bsd profile list
  Name:               gdb
  Flake attribute:    legacyPackages.x86_64-linux.gdb
  Original flake URL: flake:bsdpkgs
  Locked flake URL:   github:BasedLinux/bsdpkgs/7b38b03d76ab71bdc8dc325e3f6338d984cc35ca
  Store paths:        /bsd/store/indzcw5wvlhx6vwk7k4iq29q15chvr3d-gdb-11.1

  Name:               blender-bin
  Flake attribute:    packages.x86_64-linux.default
  Original flake URL: flake:blender-bin
  Locked flake URL:   github:edolstra/bsd-warez/91f2ffee657bf834e4475865ae336e2379282d34?dir=blender
  Store paths:        /bsd/store/i798sxl3j40wpdi1rgf391id1b5klw7g-blender-bin-3.1.2
  ```

  Note that you can unambiguously rebuild a package from a profile
  through its locked flake URL and flake attribute, e.g.

  ```console
  # bsd build github:edolstra/bsd-warez/91f2ffee657bf834e4475865ae336e2379282d34?dir=blender#packages.x86_64-linux.default
  ```

  will build the package `blender-bin` shown above.

# Description

This command shows what packages are currently installed in a
profile. For each installed package, it shows the following
information:

* `Name`: A unique name used to unambiguously identify the
  package in invocations of `bsd profile remove` and `bsd profile
  upgrade`.

* `Index`: An integer that can be used to unambiguously identify the
  package in invocations of `bsd profile remove` and `bsd profile upgrade`.
  (*Deprecated, will be removed in a future version in favor of `Name`.*)

* `Flake attribute`: The flake output attribute path that provides the
  package (e.g. `packages.x86_64-linux.hello`).

* `Original flake URL`: The original ("unlocked") flake reference
  specified by the user when the package was first installed via `bsd
  profile install`.

* `Locked flake URL`: The locked flake reference to which the original
  flake reference was resolved.

* `Store paths`: The store path(s) of the package.

)""
