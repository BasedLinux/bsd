# Release 1.1 (2012-07-18)

This release has the following improvements:

  - On Linux, when doing a chroot build, Bsd now uses various namespace
    features provided by the Linux kernel to improve build isolation.
    Namely:
    
      - The private network namespace ensures that builders cannot talk
        to the outside world (or vice versa): each build only sees a
        private loopback interface. This also means that two concurrent
        builds can listen on the same port (e.g. as part of a test)
        without conflicting with each other.
    
      - The PID namespace causes each build to start as PID 1. Processes
        outside of the chroot are not visible to those on the inside. On
        the other hand, processes inside the chroot *are* visible from
        the outside (though with different PIDs).
    
      - The IPC namespace prevents the builder from communicating with
        outside processes using SysV IPC mechanisms (shared memory,
        message queues, semaphores). It also ensures that all IPC
        objects are destroyed when the builder exits.
    
      - The UTS namespace ensures that builders see a hostname of
        `localhost` rather than the actual hostname.
    
      - The private mount namespace was already used by Bsd to ensure
        that the bind-mounts used to set up the chroot are cleaned up
        automatically.

  - Build logs are now compressed using `bzip2`. The command `bsd-store
                    -l` decompresses them on the fly. This can be disabled by setting
    the option `build-compress-log` to `false`.

  - The creation of build logs in `/bsd/var/log/bsd/drvs` can be
    disabled by setting the new option `build-keep-log` to `false`. This
    is useful, for instance, for Hydra build machines.

  - Bsd now reserves some space in `/bsd/var/bsd/db/reserved` to ensure
    that the garbage collector can run successfully if the disk is full.
    This is necessary because SQLite transactions fail if the disk is
    full.

  - Added a basic `fetchurl` function. This is not intended to replace
    the `fetchurl` in Bsdpkgs, but is useful for bootstrapping; e.g., it
    will allow us to get rid of the bootstrap binaries in the Bsdpkgs
    source tree and download them instead. You can use it by doing
    `import <bsd/fetchurl.bsd> { url =
                    url; sha256 =
                    "hash"; }`. (Shea Levy)

  - Improved RPM spec file. (Michel Alexandre Salim)

  - Support for on-demand socket-based activation in the Bsd daemon with
    `systemd`.

  - Added a manpage for bsd.conf5.

  - When using the Bsd daemon, the `-s` flag in `bsd-env -qa` is now
    much faster.
