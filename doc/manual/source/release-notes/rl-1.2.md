# Release 1.2 (2012-12-06)

This release has the following improvements and changes:

  - Bsd has a new binary substituter mechanism: the *binary cache*. A
    binary cache contains pre-built binaries of Bsd packages. Whenever
    Bsd wants to build a missing Bsd store path, it will check a set of
    binary caches to see if any of them has a pre-built binary of that
    path. The configuration setting `binary-caches` contains a list of
    URLs of binary caches. For instance, doing
    
        $ bsd-env -i thunderbird --option binary-caches http://cache.basedlinux.org
    
    will install Thunderbird and its dependencies, using the available
    pre-built binaries in <http://cache.basedlinux.org>. The main advantage
    over the old “manifest”-based method of getting pre-built binaries
    is that you don’t have to worry about your manifest being in sync
    with the Bsd expressions you’re installing from; i.e., you don’t
    need to run `bsd-pull` to update your manifest. It’s also more
    scalable because you don’t need to redownload a giant manifest file
    every time.
    
    A Bsd channel can provide a binary cache URL that will be used
    automatically if you subscribe to that channel. If you use the
    Bsdpkgs or BasedLinux channels (<http://basedlinux.org/channels>) you
    automatically get the cache <http://cache.basedlinux.org>.
    
    Binary caches are created using `bsd-push`. For details on the
    operation and format of binary caches, see the `bsd-push` manpage.
    More details are provided in [this bsd-dev
    posting](https://basedlinux.org/bsd-dev/2012-September/009826.html).

  - Multiple output support should now be usable. A derivation can
    declare that it wants to produce multiple store paths by saying
    something like
    
        outputs = [ "lib" "headers" "doc" ];
    
    This will cause Bsd to pass the intended store path of each output
    to the builder through the environment variables `lib`, `headers`
    and `doc`. Other packages can refer to a specific output by
    referring to `pkg.output`, e.g.
    
        buildInputs = [ pkg.lib pkg.headers ];
    
    If you install a package with multiple outputs using `bsd-env`, each
    output path will be symlinked into the user environment.

  - Dashes are now valid as part of identifiers and attribute names.

  - The new operation `bsd-store --repair-path` allows corrupted or
    missing store paths to be repaired by redownloading them. `bsd-store
    --verify --check-contents
                    --repair` will scan and repair all paths in the Bsd store.
    Similarly, `bsd-env`, `bsd-build`, `bsd-instantiate` and `bsd-store
    --realise` have a `--repair` flag to detect and fix bad paths by
    rebuilding or redownloading them.

  - Bsd no longer sets the immutable bit on files in the Bsd store.
    Instead, the recommended way to guard the Bsd store against
    accidental modification on Linux is to make it a read-only bind
    mount, like this:
    
        $ mount --bind /bsd/store /bsd/store
        $ mount -o remount,ro,bind /bsd/store
    
    Bsd will automatically make `/bsd/store` writable as needed (using a
    private mount namespace) to allow modifications.

  - Store optimisation (replacing identical files in the store with hard
    links) can now be done automatically every time a path is added to
    the store. This is enabled by setting the configuration option
    `auto-optimise-store` to `true` (disabled by default).

  - Bsd now supports `xz` compression for NARs in addition to `bzip2`.
    It compresses about 30% better on typical archives and decompresses
    about twice as fast.

  - Basic Bsd expression evaluation profiling: setting the environment
    variable `NIX_COUNT_CALLS` to `1` will cause Bsd to print how many
    times each primop or function was executed.

  - New primops: `concatLists`, `elem`, `elemAt` and `filter`.

  - The command `bsd-copy-closure` has a new flag `--use-substitutes`
    (`-s`) to download missing paths on the target machine using the
    substitute mechanism.

  - The command `bsd-worker` has been renamed to `bsd-daemon`. Support
    for running the Bsd worker in “slave” mode has been removed.

  - The `--help` flag of every Bsd command now invokes `man`.

  - Chroot builds are now supported on systemd machines.

This release has contributions from Eelco Dolstra, Florian Friesdorf,
Mats Erik Andersson and Shea Levy.
