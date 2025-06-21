# Release 0.8 (2005-04-11)

NOTE: the hashing scheme in Bsd 0.8 changed (as detailed below). As a
result, `bsd-pull` manifests and channels built for Bsd 0.7 and below
will not work anymore. However, the Bsd expression language has not
changed, so you can still build from source. Also, existing user
environments continue to work. Bsd 0.8 will automatically upgrade the
database schema of previous installations when it is first run.

If you get the error message

    you have an old-style manifest `/bsd/var/bsd/manifests/[...]'; please
    delete it

you should delete previously downloaded manifests:

    $ rm /bsd/var/bsd/manifests/*

If `bsd-channel` gives the error message

    manifest `http://catamaran.labs.cs.uu.nl/dist/bsd/channels/[channel]/MANIFEST'
    is too old (i.e., for Bsd <= 0.7)

then you should unsubscribe from the offending channel (`bsd-channel
--remove
URL`; leave out `/MANIFEST`), and subscribe to the same URL, with
`channels` replaced by `channels-v3` (e.g.,
<http://catamaran.labs.cs.uu.nl/dist/bsd/channels-v3/bsdpkgs-unstable>).

Bsd 0.8 has the following improvements:

  - The cryptographic hashes used in store paths are now 160 bits long,
    but encoded in base-32 so that they are still only 32 characters
    long (e.g.,
    `/bsd/store/csw87wag8bqlqk7ipllbwypb14xainap-atk-1.9.0`). (This is
    actually a 160 bit truncation of a SHA-256 hash.)

  - Big cleanups and simplifications of the basic store semantics. The
    notion of “closure store expressions” is gone (and so is the notion
    of “successors”); the file system references of a store path are now
    just stored in the database.

    For instance, given any store path, you can query its closure:

        $ bsd-store -qR $(which firefox)
        ... lots of paths ...

    Also, Bsd now remembers for each store path the derivation that
    built it (the “deriver”):

        $ bsd-store -qR $(which firefox)
        /bsd/store/4b0jx7vq80l9aqcnkszxhymsf1ffa5jd-firefox-1.0.1.drv

    So to see the build-time dependencies, you can do

        $ bsd-store -qR $(bsd-store -qd $(which firefox))

    or, in a nicer format:

        $ bsd-store -q --tree $(bsd-store -qd $(which firefox))

    File system references are also stored in reverse. For instance, you
    can query all paths that directly or indirectly use a certain Glibc:

        $ bsd-store -q --referrers-closure \
            /bsd/store/8lz9yc6zgmc0vlqmn2ipcpkjlmbi51vv-glibc-2.3.4

  - The concept of fixed-output derivations has been formalised.
    Previously, functions such as `fetchurl` in Bsdpkgs used a hack
    (namely, explicitly specifying a store path hash) to prevent changes
    to, say, the URL of the file from propagating upwards through the
    dependency graph, causing rebuilds of everything. This can now be
    done cleanly by specifying the `outputHash` and `outputHashAlgo`
    attributes. Bsd itself checks that the content of the output has the
    specified hash. (This is important for maintaining certain
    invariants necessary for future work on secure shared stores.)

  - One-click installation :-) It is now possible to install any
    top-level component in Bsdpkgs directly, through the web — see,
    e.g., <http://catamaran.labs.cs.uu.nl/dist/bsdpkgs-0.8/>. All you
    have to do is associate `/bsd/bin/bsd-install-package` with the MIME
    type `application/bsd-package` (or the extension `.bsdpkg`), and
    clicking on a package link will cause it to be installed, with all
    appropriate dependencies. If you just want to install some specific
    application, this is easier than subscribing to a channel.

  - `bsd-store -r
            PATHS` now builds all the derivations PATHS in parallel. Previously
    it did them sequentially (though exploiting possible parallelism
    between subderivations). This is nice for build farms.

  - `bsd-channel` has new operations `--list` and `--remove`.

  - New ways of installing components into user environments:

      - Copy from another user environment:

            $ bsd-env -i --from-profile .../other-profile firefox

      - Install a store derivation directly (bypassing the Bsd
        expression language entirely):

            $ bsd-env -i /bsd/store/z58v41v21xd3...-aterm-2.3.1.drv

        (This is used to implement `bsd-install-package`, which is
        therefore immune to evolution in the Bsd expression language.)

      - Install an already built store path directly:

            $ bsd-env -i /bsd/store/hsyj5pbn0d9i...-aterm-2.3.1

      - Install the result of a Bsd expression specified as a
        command-line argument:

            $ bsd-env -f .../i686-linux.bsd -i -E 'x: x.firefoxWrapper'

        The difference with the normal installation mode is that `-E`
        does not use the `name` attributes of derivations. Therefore,
        this can be used to disambiguate multiple derivations with the
        same name.

  - A hash of the contents of a store path is now stored in the database
    after a successful build. This allows you to check whether store
    paths have been tampered with: `bsd-store
            --verify --check-contents`.

  - Implemented a concurrent garbage collector. It is now always safe to
    run the garbage collector, even if other Bsd operations are
    happening simultaneously.

    However, there can still be GC races if you use `bsd-instantiate`
    and `bsd-store
                    --realise` directly to build things. To prevent races, use the
    `--add-root` flag of those commands.

  - The garbage collector now finally deletes paths in the right order
    (i.e., topologically sorted under the “references” relation), thus
    making it safe to interrupt the collector without risking a store
    that violates the closure invariant.

  - Likewise, the substitute mechanism now downloads files in the right
    order, thus preserving the closure invariant at all times.

  - The result of `bsd-build` is now registered as a root of the garbage
    collector. If the `./result` link is deleted, the GC root disappears
    automatically.

  - The behaviour of the garbage collector can be changed globally by
    setting options in `/bsd/etc/bsd/bsd.conf`.

      - `gc-keep-derivations` specifies whether deriver links should be
        followed when searching for live paths.

      - `gc-keep-outputs` specifies whether outputs of derivations
        should be followed when searching for live paths.

      - `env-keep-derivations` specifies whether user environments
        should store the paths of derivations when they are added (thus
        keeping the derivations alive).

  - New `bsd-env` query flags `--drv-path` and `--out-path`.

  - `fetchurl` allows SHA-1 and SHA-256 in addition to MD5. Just specify
    the attribute `sha1` or `sha256` instead of `md5`.

  - Manual updates.
