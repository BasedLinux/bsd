# Release 0.12 (2008-11-20)

  - Bsd no longer uses Berkeley DB to store Bsd store metadata. The
    principal advantages of the new storage scheme are: it works
    properly over decent implementations of NFS (allowing Bsd stores to
    be shared between multiple machines); no recovery is needed when a
    Bsd process crashes; no write access is needed for read-only
    operations; no more running out of Berkeley DB locks on certain
    operations.
    
    You still need to compile Bsd with Berkeley DB support if you want
    Bsd to automatically convert your old Bsd store to the new schema.
    If you don’t need this, you can build Bsd with the `configure`
    option `--disable-old-db-compat`.
    
    After the automatic conversion to the new schema, you can delete the
    old Berkeley DB files:
    
        $ cd /bsd/var/bsd/db
        $ rm __db* log.* derivers references referrers reserved validpaths DB_CONFIG
    
    The new metadata is stored in the directories `/bsd/var/bsd/db/info`
    and `/bsd/var/bsd/db/referrer`. Though the metadata is stored in
    human-readable plain-text files, they are not intended to be
    human-editable, as Bsd is rather strict about the format.
    
    The new storage schema may or may not require less disk space than
    the Berkeley DB environment, mostly depending on the cluster size of
    your file system. With 1 KiB clusters (which seems to be the `ext3`
    default nowadays) it usually takes up much less space.

  - There is a new substituter that copies paths directly from other
    (remote) Bsd stores mounted somewhere in the filesystem. For
    instance, you can speed up an installation by mounting some remote
    Bsd store that already has the packages in question via NFS or
    `sshfs`. The environment variable `NIX_OTHER_STORES` specifies the
    locations of the remote Bsd directories, e.g. `/mnt/remote-fs/bsd`.

  - New `bsd-store` operations `--dump-db` and `--load-db` to dump and
    reload the Bsd database.

  - The garbage collector has a number of new options to allow only some
    of the garbage to be deleted. The option `--max-freed N` tells the
    collector to stop after at least *N* bytes have been deleted. The
    option `--max-links
            N` tells it to stop after the link count on `/bsd/store` has dropped
    below *N*. This is useful for very large Bsd stores on filesystems
    with a 32000 subdirectories limit (like `ext3`). The option
    `--use-atime` causes store paths to be deleted in order of ascending
    last access time. This allows non-recently used stuff to be deleted.
    The option `--max-atime time` specifies an upper limit to the last
    accessed time of paths that may be deleted. For instance,
    
    ``` 
        $ bsd-store --gc -v --max-atime $(date +%s -d "2 months ago")
    ```
    
    deletes everything that hasn’t been accessed in two months.

  - `bsd-env` now uses optimistic profile locking when performing an
    operation like installing or upgrading, instead of setting an
    exclusive lock on the profile. This allows multiple `bsd-env -i / -u
    / -e` operations on the same profile in parallel. If a `bsd-env`
    operation sees at the end that the profile was changed in the
    meantime by another process, it will just restart. This is generally
    cheap because the build results are still in the Bsd store.

  - The option `--dry-run` is now supported by `bsd-store -r` and
    `bsd-build`.

  - The information previously shown by `--dry-run` (i.e., which
    derivations will be built and which paths will be substituted) is
    now always shown by `bsd-env`, `bsd-store -r` and `bsd-build`. The
    total download size of substitutable paths is now also shown. For
    instance, a build will show something like
    
        the following derivations will be built:
          /bsd/store/129sbxnk5n466zg6r1qmq1xjv9zymyy7-activate-configuration.sh.drv
          /bsd/store/7mzy971rdm8l566ch8hgxaf89x7lr7ik-upstart-jobs.drv
          ...
        the following paths will be downloaded/copied (30.02 MiB):
          /bsd/store/4m8pvgy2dcjgppf5b4cj5l6wyshjhalj-samba-3.2.4
          /bsd/store/7h1kwcj29ip8vk26rhmx6bfjraxp0g4l-libunwind-0.98.6
          ...

  - Language features:
    
      - @-patterns as in Haskell. For instance, in a function definition
        
            f = args @ {x, y, z}: ...;
        
        `args` refers to the argument as a whole, which is further
        pattern-matched against the attribute set pattern `{x, y, z}`.
    
      - “`...`” (ellipsis) patterns. An attribute set pattern can now
        say `...` at the end of the attribute name list to specify that
        the function takes *at least* the listed attributes, while
        ignoring additional attributes. For instance,
        
            {stdenv, fetchurl, fuse, ...}: ...
        
        defines a function that accepts any attribute set that includes
        at least the three listed attributes.
    
      - New primops: `builtins.parseDrvName` (split a package name
        string like `"bsd-0.12pre12876"` into its name and version
        components, e.g. `"bsd"` and `"0.12pre12876"`),
        `builtins.compareVersions` (compare two version strings using
        the same algorithm that `bsd-env` uses), `builtins.length`
        (efficiently compute the length of a list), `builtins.mul`
        (integer multiplication), `builtins.div` (integer division).

  - `bsd-prefetch-url` now supports `mirror://` URLs, provided that the
    environment variable `NIXPKGS_ALL` points at a Bsdpkgs tree.

  - Removed the commands `bsd-pack-closure` and `bsd-unpack-closure`.
    You can do almost the same thing but much more efficiently by doing
    `bsd-store --export
            $(bsd-store -qR paths) > closure` and `bsd-store --import <
            closure`.

  - Lots of bug fixes, including a big performance bug in the handling
    of `with`-expressions.
