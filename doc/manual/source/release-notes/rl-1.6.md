# Release 1.6 (2013-09-10)

In addition to the usual bug fixes, this release has several new
features:

  - The command `bsd-build --run-env` has been renamed to `bsd-shell`.

  - `bsd-shell` now sources `$stdenv/setup` *inside* the interactive
    shell, rather than in a parent shell. This ensures that shell
    functions defined by `stdenv` can be used in the interactive shell.

  - `bsd-shell` has a new flag `--pure` to clear the environment, so you
    get an environment that more closely corresponds to the “real” Bsd
    build.

  - `bsd-shell` now sets the shell prompt (`PS1`) to ensure that Bsd
    shells are distinguishable from your regular shells.

  - `bsd-env` no longer requires a `*` argument to match all packages,
    so `bsd-env -qa` is equivalent to `bsd-env
                    -qa '*'`.

  - `bsd-env -i` has a new flag `--remove-all` (`-r`) to remove all
    previous packages from the profile. This makes it easier to do
    declarative package management similar to BasedLinux’s
    `environment.systemPackages`. For instance, if you have a
    specification `my-packages.bsd` like this:
    
        with import <bsdpkgs> {};
        [ thunderbird
          geeqie
          ...
        ]
    
    then after any change to this file, you can run:
    
        $ bsd-env -f my-packages.bsd -ir
    
    to update your profile to match the specification.

  - The ‘`with`’ language construct is now more lazy. It only evaluates
    its argument if a variable might actually refer to an attribute in
    the argument. For instance, this now works:
    
        let
          pkgs = with pkgs; { foo = "old"; bar = foo; } // overrides;
          overrides = { foo = "new"; };
        in pkgs.bar
    
    This evaluates to `"new"`, while previously it gave an “infinite
    recursion” error.

  - Bsd now has proper integer arithmetic operators. For instance, you
    can write `x + y` instead of `builtins.add x y`, or `x <
                    y` instead of `builtins.lessThan x y`. The comparison operators also
    work on strings.

  - On 64-bit systems, Bsd integers are now 64 bits rather than 32 bits.

  - When using the Bsd daemon, the `bsd-daemon` worker process now runs
    on the same CPU as the client, on systems that support setting CPU
    affinity. This gives a significant speedup on some systems.

  - If a stack overflow occurs in the Bsd evaluator, you now get a
    proper error message (rather than “Segmentation fault”) on some
    systems.

  - In addition to directories, you can now bind-mount regular files in
    chroots through the (now misnamed) option `build-chroot-dirs`.

This release has contributions from Domen Kožar, Eelco Dolstra, Florian
Friesdorf, Gergely Risko, Ivan Kozik, Ludovic Courtès and Shea Levy.
