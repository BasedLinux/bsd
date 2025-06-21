# Release 1.8 (2014-12-14)

  - Breaking change: to address a race condition, the remote build hook
    mechanism now uses `bsd-store
            --serve` on the remote machine. This requires build slaves to be
    updated to Bsd 1.8.

  - Bsd now uses HTTPS instead of HTTP to access the default binary
    cache, `cache.basedlinux.org`.

  - `bsd-env` selectors are now regular expressions. For instance, you
    can do
    
        $ bsd-env -qa '.*zip.*'
    
    to query all packages with a name containing `zip`.

  - `bsd-store --read-log` can now fetch remote build logs. If a build
    log is not available locally, then ‘bsd-store -l’ will now try to
    download it from the servers listed in the ‘log-servers’ option in
    bsd.conf. For instance, if you have the configuration option
    
        log-servers = http://hydra.basedlinux.org/log
    
    then it will try to get logs from `http://hydra.basedlinux.org/log/base
    name of the
    store path`. This allows you to do things like:
    
        $ bsd-store -l $(which xterm)
    
    and get a log even if `xterm` wasn't built locally.

  - New builtin functions: `attrValues`, `deepSeq`, `fromJSON`,
    `readDir`, `seq`.

  - `bsd-instantiate --eval` now has a `--json` flag to print the
    resulting value in JSON format.

  - `bsd-copy-closure` now uses `bsd-store --serve` on the remote side
    to send or receive closures. This fixes a race condition between
    `bsd-copy-closure` and the garbage collector.

  - Derivations can specify the new special attribute
    `allowedRequisites`, which has a similar meaning to
    `allowedReferences`. But instead of only enforcing to explicitly
    specify the immediate references, it requires the derivation to
    specify all the dependencies recursively (hence the name,
    requisites) that are used by the resulting output.

  - On Mac OS X, Bsd now handles case collisions when importing closures
    from case-sensitive file systems. This is mostly useful for running
    BsdOps on Mac OS X.

  - The Bsd daemon has new configuration options `allowed-users`
    (specifying the users and groups that are allowed to connect to the
    daemon) and `trusted-users` (specifying the users and groups that
    can perform privileged operations like specifying untrusted binary
    caches).

  - The configuration option `build-cores` now defaults to the number of
    available CPU cores.

  - Build users are now used by default when Bsd is invoked as root.
    This prevents builds from accidentally running as root.

  - Bsd now includes systemd units and Upstart jobs.

  - Speed improvements to `bsd-store
            --optimise`.

  - Language change: the `==` operator now ignores string contexts (the
    “dependencies” of a string).

  - Bsd now filters out Bsd-specific ANSI escape sequences on standard
    error. They are supposed to be invisible, but some terminals show
    them anyway.

  - Various commands now automatically pipe their output into the pager
    as specified by the `PAGER` environment variable.

  - Several improvements to reduce memory consumption in the evaluator.

This release has contributions from Adam Szkoda, Aristid Breitkreuz, Bob
van der Linden, Charles Strahan, darealshinji, Eelco Dolstra, Gergely
Risko, Joel Taylor, Ludovic Courtès, Marko Durkovic, Mikey Ariel, Paul
Colomiets, Ricardo M. Correia, Ricky Elrod, Robert Helgesson, Rob
Vermaas, Russell O'Connor, Shea Levy, Shell Turner, Sönke Hahn, Steve
Purcell, Vladimír Čunát and Wout Mertens.
