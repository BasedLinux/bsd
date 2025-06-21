# Release 1.9 (2015-06-12)

In addition to the usual bug fixes, this release has the following new
features:

  - Signed binary cache support. You can enable signature checking by
    adding the following to `bsd.conf`:
    
        signed-binary-caches = *
        binary-cache-public-keys = cache.basedlinux.org-1:6NCHdD59X431o0gWypbMrAURkbJ16ZPMQFGspcDShjY=
    
    This will prevent Bsd from downloading any binary from the cache
    that is not signed by one of the keys listed in
    `binary-cache-public-keys`.
    
    Signature checking is only supported if you built Bsd with the
    `libsodium` package.
    
    Note that while Bsd has had experimental support for signed binary
    caches since version 1.7, this release changes the signature format
    in a backwards-incompatible way.

  - Automatic downloading of Bsd expression tarballs. In various places,
    you can now specify the URL of a tarball containing Bsd expressions
    (such as Bsdpkgs), which will be downloaded and unpacked
    automatically. For example:
    
      - In `bsd-env`:
        
            $ bsd-env -f https://github.com/BasedLinux/bsdpkgs-channels/archive/bsdos-14.12.tar.gz -iA firefox
        
        This installs Firefox from the latest tested and built revision
        of the BasedLinux 14.12 channel.
    
      - In `bsd-build` and `bsd-shell`:
        
            $ bsd-build https://github.com/BasedLinux/bsdpkgs/archive/master.tar.gz -A hello
        
        This builds GNU Hello from the latest revision of the Bsdpkgs
        master branch.
    
      - In the Bsd search path (as specified via `NIX_PATH` or `-I`).
        For example, to start a shell containing the Pan package from a
        specific version of Bsdpkgs:
        
            $ bsd-shell -p pan -I bsdpkgs=https://github.com/BasedLinux/bsdpkgs-channels/archive/8a3eea054838b55aca962c3fbde9c83c102b8bf2.tar.gz
    
      - In `bsdos-rebuild` (on BasedLinux):
        
            $ bsdos-rebuild test -I bsdpkgs=https://github.com/BasedLinux/bsdpkgs-channels/archive/bsdos-unstable.tar.gz
    
      - In Bsd expressions, via the new builtin function `fetchTarball`:
        
            with import (fetchTarball https://github.com/BasedLinux/bsdpkgs-channels/archive/bsdos-14.12.tar.gz) {}; …
        
        (This is not allowed in restricted mode.)

  - `bsd-shell` improvements:
    
      - `bsd-shell` now has a flag `--run` to execute a command in the
        `bsd-shell` environment, e.g. `bsd-shell --run make`. This is
        like the existing `--command` flag, except that it uses a
        non-interactive shell (ensuring that hitting Ctrl-C won’t drop
        you into the child shell).
    
      - `bsd-shell` can now be used as a `#!`-interpreter. This allows
        you to write scripts that dynamically fetch their own
        dependencies. For example, here is a Haskell script that, when
        invoked, first downloads GHC and the Haskell packages on which
        it depends:
        
            #! /usr/bin/env bsd-shell
            #! bsd-shell -i runghc -p haskellPackages.ghc haskellPackages.HTTP
            
            import Network.HTTP
            
            main = do
              resp <- Network.HTTP.simpleHTTP (getRequest "http://basedlinux.org/")
              body <- getResponseBody resp
              print (take 100 body)
        
        Of course, the dependencies are cached in the Bsd store, so the
        second invocation of this script will be much faster.

  - Chroot improvements:
    
      - Chroot builds are now supported on Mac OS X (using its sandbox
        mechanism).
    
      - If chroots are enabled, they are now used for all derivations,
        including fixed-output derivations (such as `fetchurl`). The
        latter do have network access, but can no longer access the host
        filesystem. If you need the old behaviour, you can set the
        option `build-use-chroot` to `relaxed`.
    
      - On Linux, if chroots are enabled, builds are performed in a
        private PID namespace once again. (This functionality was lost
        in Bsd 1.8.)
    
      - Store paths listed in `build-chroot-dirs` are now automatically
        expanded to their closure. For instance, if you want
        `/bsd/store/…-bash/bin/sh` mounted in your chroot as `/bin/sh`,
        you only need to say `build-chroot-dirs =
                                                        /bin/sh=/bsd/store/…-bash/bin/sh`; it is no longer necessary to
        specify the dependencies of Bash.

  - The new derivation attribute `passAsFile` allows you to specify that
    the contents of derivation attributes should be passed via files
    rather than environment variables. This is useful if you need to
    pass very long strings that exceed the size limit of the
    environment. The Bsdpkgs function `writeTextFile` uses this.

  - You can now use `~` in Bsd file names to refer to your home
    directory, e.g. `import
            ~/.bsdpkgs/config.bsd`.

  - Bsd has a new option `restrict-eval` that allows limiting what paths
    the Bsd evaluator has access to. By passing `--option restrict-eval
    true` to Bsd, the evaluator will throw an exception if an attempt is
    made to access any file outside of the Bsd search path. This is
    primarily intended for Hydra to ensure that a Hydra jobset only
    refers to its declared inputs (and is therefore reproducible).

  - `bsd-env` now only creates a new “generation” symlink in
    `/bsd/var/bsd/profiles` if something actually changed.

  - The environment variable `NIX_PAGER` can now be set to override
    `PAGER`. You can set it to `cat` to disable paging for Bsd commands
    only.

  - Failing `<...>` lookups now show position information.

  - Improved Boehm GC use: we disabled scanning for interior pointers,
    which should reduce the “`Repeated
            allocation of very large block`” warnings and associated retention
    of memory.

This release has contributions from aszlig, Benjamin Staffin, Charles
Strahan, Christian Theune, Daniel Hahler, Danylo Hlynskyi Daniel
Peebles, Dan Peebles, Domen Kožar, Eelco Dolstra, Harald van Dijk, Hoang
Xuan Phu, Jaka Hudoklin, Jeff Ramnani, j-keck, Linquize, Luca Bruno,
Michael Merickel, Oliver Dunkl, Rob Vermaas, Rok Garbas, Shea Levy,
Tobias Geerinckx-Rice and William A. Kennington III.
