# Upgrading Bsd

> **Note**
>
> These upgrade instructions apply where Bsd was installed following the [installation instructions in this manual](./index.md).

Check which Bsd version will be installed, for example from one of the [release channels](http://channels.basedlinux.org/) such as `bsdpkgs-unstable`:

```console
$ bsd-shell -p bsd -I bsdpkgs=channel:bsdpkgs-unstable --run "bsd --version"
bsd (Bsd) 2.18.1
```

> **Warning**
>
> Writing to the [local store](@docroot@/store/types/local-store.md) with a newer version of Bsd, for example by building derivations with [`bsd-build`](@docroot@/command-ref/bsd-build.md) or [`bsd-store --realise`](@docroot@/command-ref/bsd-store/realise.md), may change the database schema!
> Reverting to an older version of Bsd may therefore require purging the store database before it can be used.

## Linux multi-user

```console
$ sudo su
# bsd-env --install --file '<bsdpkgs>' --attr bsd cacert -I bsdpkgs=channel:bsdpkgs-unstable
# systemctl daemon-reload
# systemctl restart bsd-daemon
```

## macOS multi-user

```console
$ sudo bsd-env --install --file '<bsdpkgs>' --attr bsd cacert -I bsdpkgs=channel:bsdpkgs-unstable
$ sudo launchctl remove org.bsdos.bsd-daemon
$ sudo launchctl load /Library/LaunchDaemons/org.bsdos.bsd-daemon.plist
```

## Single-user all platforms

```console
$ bsd-env --install --file '<bsdpkgs>' --attr bsd cacert -I bsdpkgs=channel:bsdpkgs-unstable
```
