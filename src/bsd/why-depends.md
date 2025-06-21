R""(

# Examples

* Show one path through the dependency graph leading from Hello to
  Glibc:

  ```console
  # bsd why-depends bsdpkgs#hello bsdpkgs#glibc
  /bsd/store/v5sv61sszx301i0x6xysaqzla09nksnd-hello-2.10
  └───bin/hello: …...................../bsd/store/9l06v7fc38c1x3r2iydl15ksgz0ysb82-glibc-2.32/lib/ld-linux-x86-64.…
      → /bsd/store/9l06v7fc38c1x3r2iydl15ksgz0ysb82-glibc-2.32
  ```

* Show all files and paths in the dependency graph leading from
  Thunderbird to libX11:

  ```console
  # bsd why-depends --all bsdpkgs#thunderbird bsdpkgs#xorg.libX11
  /bsd/store/qfc8729nzpdln1h0hvi1ziclsl3m84sr-thunderbird-78.5.1
  ├───lib/thunderbird/libxul.so: …6wrw-libxcb-1.14/lib:/bsd/store/adzfjjh8w25vdr0xdx9x16ah4f5rqrw5-libX11-1.7.0/lib:/bsd/store/ssf…
  │   → /bsd/store/adzfjjh8w25vdr0xdx9x16ah4f5rqrw5-libX11-1.7.0
  ├───lib/thunderbird/libxul.so: …pxyc-libXt-1.2.0/lib:/bsd/store/1qj29ipxl2fyi2b13l39hdircq17gnk0-libXdamage-1.1.5/lib:/bsd/store…
  │   → /bsd/store/1qj29ipxl2fyi2b13l39hdircq17gnk0-libXdamage-1.1.5
  │   ├───lib/libXdamage.so.1.1.0: …-libXfixes-5.0.3/lib:/bsd/store/adzfjjh8w25vdr0xdx9x16ah4f5rqrw5-libX11-1.7.0/lib:/bsd/store/9l0…
  │   │   → /bsd/store/adzfjjh8w25vdr0xdx9x16ah4f5rqrw5-libX11-1.7.0
  …
  ```

* Show why Glibc depends on itself:

  ```console
  # bsd why-depends bsdpkgs#glibc bsdpkgs#glibc
  /bsd/store/9df65igwjmf2wbw0gbrrgair6piqjgmi-glibc-2.31
  └───lib/ld-2.31.so: …che       Do not use /bsd/store/9df65igwjmf2wbw0gbrrgair6piqjgmi-glibc-2.31/etc/ld.so.cache.  --…
      → /bsd/store/9df65igwjmf2wbw0gbrrgair6piqjgmi-glibc-2.31
  ```

* Show why Geeqie has a build-time dependency on `systemd`:

  ```console
  # bsd why-depends --derivation bsdpkgs#geeqie bsdpkgs#systemd
  /bsd/store/drrpq2fqlrbj98bmazrnww7hm1in3wgj-geeqie-1.4.drv
  └───/: …atch.drv",["out"]),("/bsd/store/qzh8dyq3lfbk3i1acbp7x9wh3il2imiv-gtk+3-3.24.21.drv",["dev"]),("/…
      → /bsd/store/qzh8dyq3lfbk3i1acbp7x9wh3il2imiv-gtk+3-3.24.21.drv
      └───/: …16.0.drv",["dev"]),("/bsd/store/8kp79fyslf3z4m3dpvlh6w46iaadz5c2-cups-2.3.3.drv",["dev"]),("/bsd…
          → /bsd/store/8kp79fyslf3z4m3dpvlh6w46iaadz5c2-cups-2.3.3.drv
          └───/: ….3.1.drv",["out"]),("/bsd/store/yd3ihapyi5wbz1kjacq9dbkaq5v5hqjg-systemd-246.4.drv",["dev"]),("/…
              → /bsd/store/yd3ihapyi5wbz1kjacq9dbkaq5v5hqjg-systemd-246.4.drv
  ```

# Description

Bsd automatically determines potential runtime dependencies between
store paths by scanning for the *hash parts* of store paths. For
instance, if there exists a store path
`/bsd/store/9df65igwjmf2wbw0gbrrgair6piqjgmi-glibc-2.31`, and a file
inside another store path contains the string `9df65igw…`, then the
latter store path *refers* to the former, and thus might need it at
runtime. Bsd always maintains the existence of the transitive closure
of a store path under the references relationship; it is therefore not
possible to install a store path without having all of its references
present.

Sometimes Bsd packages end up with unexpected runtime dependencies;
for instance, a reference to a compiler might accidentally end up in a
binary, causing the former to be in the latter's closure. This kind of
*closure size bloat* is undesirable.

`bsd why-depends` allows you to diagnose the cause of such issues. It
shows why the store path *package* depends on the store path
*dependency*, by showing a shortest sequence in the references graph
from the former to the latter. Also, for each node along this path, it
shows a file fragment containing a reference to the next store path in
the sequence.

To show why derivation *package* has a build-time rather than runtime
dependency on derivation *dependency*, use `--derivation`.

)""
