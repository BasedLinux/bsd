# Release 2.7 (2022-03-07)

* Bsd will now make some helpful suggestions when you mistype
  something on the command line. For instance, if you type `bsd build
  bsdpkgs#thunderbrd`, it will suggest `thunderbird`.

* A number of "default" flake output attributes have been
  renamed. These are:

  * `defaultPackage.<system>` → `packages.<system>.default`
  * `defaultApps.<system>` → `apps.<system>.default`
  * `defaultTemplate` → `templates.default`
  * `defaultBundler.<system>` → `bundlers.<system>.default`
  * `overlay` → `overlays.default`
  * `devShell.<system>` → `devShells.<system>.default`

  The old flake output attributes still work, but `bsd flake check`
  will warn about them.

* Breaking API change: `bsd bundle` now supports bundlers of the form
  `bundler.<system>.<name>= derivation: another-derivation;`. This
  supports additional functionality to inspect evaluation information
  during bundling. A new
  [repository](https://github.com/BasedLinux/bundlers) has various bundlers
  implemented.

* `bsd store ping` now reports the version of the remote Bsd daemon.

* `bsd flake {init,new}` now display information about which files have been
  created.

* Templates can now define a `welcomeText` attribute, which is printed out by
  `bsd flake {init,new} --template <template>`.
