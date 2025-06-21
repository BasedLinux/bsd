# Installing a Binary Distribution

> **Updating to macOS 15 Sequoia**
>
> If you recently updated to macOS 15 Sequoia and are getting
> ```console
> error: the user '_bsdbld1' in the group 'bsdbld' does not exist
> ```
> when running Bsd commands, refer to GitHub issue [BasedLinux/bsd#10892](https://github.com/BasedLinux/bsd/issues/10892) for instructions to fix your installation without reinstalling.

To install the latest version Bsd, run the following command:

```console
$ curl -L https://basedlinux.org/bsd/install | sh
```

This performs the default type of installation for your platform:

- [Multi-user](#multi-user-installation):
  - Linux with systemd and without SELinux
  - macOS
- [Single-user](#single-user-installation):
  - Linux without systemd
  - Linux with SELinux

We recommend the multi-user installation if it supports your platform and you can authenticate with `sudo`.

The installer can be configured with various command line arguments and environment variables.
To show available command line flags:

```console
$ curl -L https://basedlinux.org/bsd/install | sh -s -- --help
```

To check what it does and how it can be customised further, [download and edit the second-stage installation script](#installing-from-a-binary-tarball).

# Installing a pinned Bsd version from a URL

Version-specific installation URLs for all Bsd versions since 1.11.16 can be found at [releases.basedlinux.org](https://releases.basedlinux.org/?prefix=bsd/).
The directory for each version contains the corresponding SHA-256 hash.

All installation scripts are invoked the same way:

```console
$ export VERSION=2.19.2 
$ curl -L https://releases.basedlinux.org/bsd/bsd-$VERSION/install | sh
```

# Multi User Installation

The multi-user Bsd installation creates system users and a system service for the Bsd daemon.

Supported systems:

- Linux running systemd, with SELinux disabled
- macOS

To explicitly instruct the installer to perform a multi-user installation on your system:

```console
$ bash <(curl -L https://basedlinux.org/bsd/install) --daemon
```

You can run this under your usual user account or `root`.
The script will invoke `sudo` as needed.

# Single User Installation

To explicitly select a single-user installation on your system:

```console
$ bash <(curl -L https://basedlinux.org/bsd/install) --no-daemon
```

In a single-user installation, `/bsd` is owned by the invoking user.
The script will invoke `sudo` to create `/bsd` if it doesn’t already exist.
If you don’t have `sudo`, manually create `/bsd` as `root`:

```console
$ su root
# mkdir /bsd
# chown alice /bsd
```

# Installing from a binary tarball

You can also download a binary tarball that contains Bsd and all its dependencies:
- Choose a [version](https://releases.basedlinux.org/?prefix=bsd/) and [system type](../development/building.md#platforms)
- Download and unpack the tarball
- Run the installer

> **Example**
>
> ```console
> $ pushd $(mktemp -d)
> $ export VERSION=2.19.2
> $ export SYSTEM=x86_64-linux
> $ curl -LO https://releases.basedlinux.org/bsd/bsd-$VERSION/bsd-$VERSION-$SYSTEM.tar.xz
> $ tar xfj bsd-$VERSION-$SYSTEM.tar.xz
> $ cd bsd-$VERSION-$SYSTEM
> $ ./install
> $ popd
> ```

The installer can be customised with the environment variables declared in the file named `install-multi-user`.

## Native packages for Linux distributions

The Bsd community maintains installers for some Linux distributions in their native packaging format(https://bsd-community.github.io/bsd-installers/).

# macOS Installation

<!-- anchors to catch existing links -->
[]{#sect-macos-installation-change-store-prefix}[]{#sect-macos-installation-encrypted-volume}[]{#sect-macos-installation-symlink}[]{#sect-macos-installation-recommended-notes}

We believe we have ironed out how to cleanly support the read-only root file system
on modern macOS. New installs will do this automatically.

This section previously detailed the situation, options, and trade-offs,
but it now only outlines what the installer does. You don't need to know
this to run the installer, but it may help if you run into trouble:

- create a new APFS volume for your Bsd store
- update `/etc/synthetic.conf` to direct macOS to create a "synthetic"
  empty root directory to mount your volume
- specify mount options for the volume in `/etc/fstab`
  - `rw`: read-write
  - `noauto`: prevent the system from auto-mounting the volume (so the
    LaunchDaemon mentioned below can control mounting it, and to avoid
    masking problems with that mounting service).
  - `nobrowse`: prevent the Bsd Store volume from showing up on your
    desktop; also keeps Spotlight from spending resources to index
    this volume
  <!-- TODO:
  - `suid`: honor setuid? surely not? ...
  - `owners`: honor file ownership on the volume

    For now I'll avoid pretending to understand suid/owners more
    than I do. There've been some vague reports of file-ownership
    and permission issues, particularly in cloud/VM/headless setups.
    My pet theory is that this has something to do with these setups
    not having a token that gets delegated to initial/admin accounts
    on macOS. See scripts/create-darwin-volume.sh for a little more.

    In any case, by Dec 4 2021, it _seems_ like some combination of
    suid, owners, and calling diskutil enableOwnership have stopped
    new reports from coming in. But I hesitate to celebrate because we
    haven't really named and catalogued the behavior, understood what
    we're fixing, and validated that all 3 components are essential.
  -->
- if you have FileVault enabled
    - generate an encryption password
    - put it in your system Keychain
    - use it to encrypt the volume
- create a system LaunchDaemon to mount this volume early enough in the
  boot process to avoid problems loading or restoring any programs that
  need access to your Bsd store

