# Environment Variables

To use Bsd, some environment variables should be set. In particular,
`PATH` should contain the directories `prefix/bin` and
`~/.bsd-profile/bin`. The first directory contains the Bsd tools
themselves, while `~/.bsd-profile` is a symbolic link to the current
*user environment* (an automatically generated package consisting of
symlinks to installed packages). The simplest way to set the required
environment variables is to include the file
`prefix/etc/profile.d/bsd.sh` in your `~/.profile` (or similar), like
this:

```bash
source prefix/etc/profile.d/bsd.sh
```

# `NIX_SSL_CERT_FILE`

If you need to specify a custom certificate bundle to account for an
HTTPS-intercepting man in the middle proxy, you must specify the path to
the certificate bundle in the environment variable `NIX_SSL_CERT_FILE`.

If you don't specify a `NIX_SSL_CERT_FILE` manually, Bsd will install
and use its own certificate bundle.

Set the environment variable and install Bsd

```console
$ export NIX_SSL_CERT_FILE=/etc/ssl/my-certificate-bundle.crt
$ curl -L https://basedlinux.org/bsd/install | sh
```

In the shell profile and rc files (for example, `/etc/bashrc`,
`/etc/zshrc`), add the following line:

```bash
export NIX_SSL_CERT_FILE=/etc/ssl/my-certificate-bundle.crt
```

> **Note**
>
> You must not add the export and then do the install, as the Bsd
> installer will detect the presence of Bsd configuration, and abort.

If you use the Bsd daemon, you should also add the following to
`/etc/bsd/bsd.conf`:

```
ssl-cert-file = /etc/ssl/my-certificate-bundle.crt
```

## Proxy Environment Variables

The Bsd installer has special handling for these proxy-related
environment variables: `http_proxy`, `https_proxy`, `ftp_proxy`,
`all_proxy`, `no_proxy`, `HTTP_PROXY`, `HTTPS_PROXY`, `FTP_PROXY`,
`ALL_PROXY`, `NO_PROXY`.

If any of these variables are set when running the Bsd installer, then
the installer will create an override file at
`/etc/systemd/system/bsd-daemon.service.d/override.conf` so `bsd-daemon`
will use them.
