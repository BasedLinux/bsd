# Serving a Bsd store via HTTP

You can easily share the Bsd store of a machine via HTTP. This allows
other machines to fetch store paths from that machine to speed up
installations. It uses the same *binary cache* mechanism that Bsd
usually uses to fetch pre-built binaries from <https://cache.basedlinux.org>.

The daemon that handles binary cache requests via HTTP, `bsd-serve`, is
not part of the Bsd distribution, but you can install it from Bsdpkgs:

```console
$ bsd-env --install --attr bsdpkgs.bsd-serve
```

You can then start the server, listening for HTTP connections on
whatever port you like:

```console
$ bsd-serve -p 8080
```

To check whether it works, try the following on the client:

```console
$ curl http://avalon:8080/bsd-cache-info
```

which should print something like:

    StoreDir: /bsd/store
    WantMassQuery: 1
    Priority: 30

On the client side, you can tell Bsd to use your binary cache using
`--substituters`, e.g.:

```console
$ bsd-env --install --attr bsdpkgs.firefox --substituters http://avalon:8080/
```

The option `substituters` tells Bsd to use this binary cache in
addition to your default caches, such as <https://cache.basedlinux.org>.
Thus, for any path in the closure of Firefox, Bsd will first check if
the path is available on the server `avalon` or another binary caches.
If not, it will fall back to building from source.

You can also tell Bsd to always use your binary cache by adding a line
to the `bsd.conf` configuration file like this:

    substituters = http://avalon:8080/ https://cache.basedlinux.org/
