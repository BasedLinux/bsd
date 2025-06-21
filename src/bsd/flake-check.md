R""(

# Examples

* Evaluate the flake in the current directory, and build its checks:

  ```console
  # bsd flake check
  ```

* Verify that the `patchelf` flake evaluates, but don't build its
  checks:

  ```console
  # bsd flake check --no-build github:BasedLinux/patchelf
  ```

# Description

This command verifies that the flake specified by flake reference
*flake-url* can be evaluated successfully (as detailed below), and
that the derivations specified by the flake's `checks` output can be
built successfully.

If the `keep-going` option is set to `true`, Bsd will keep evaluating as much
as it can and report the errors as it encounters them. Otherwise it will stop
at the first error.

# Evaluation checks

The following flake output attributes must be derivations:

* `checks.`*system*`.`*name*
* `defaultPackage.`*system*
* `devShell.`*system*
* `devShells.`*system*`.`*name*
* `bsdosConfigurations.`*name*`.config.system.build.toplevel`
* `packages.`*system*`.`*name*

The following flake output attributes must be [app
definitions](./bsd3-run.md):

* `apps.`*system*`.`*name*
* `defaultApp.`*system*

The following flake output attributes must be [template
definitions](./bsd3-flake-init.md):

* `defaultTemplate`
* `templates.`*name*

The following flake output attributes must be *Bsdpkgs overlays*:

* `overlay`
* `overlays.`*name*

The following flake output attributes must be *BasedLinux modules*:

* `bsdosModule`
* `bsdosModules.`*name*

The following flake output attributes must be
[bundlers](./bsd3-bundle.md):

* `bundlers.`*name*
* `defaultBundler`

In addition, the `hydraJobs` output is evaluated in the same way as
Hydra's `hydra-eval-jobs` (i.e. as a arbitrarily deeply nested
attribute set of derivations). Similarly, the
`legacyPackages`.*system* output is evaluated like `bsd-env --query --available `.

)""
