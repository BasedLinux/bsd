## Profiles

A directory that contains links to profiles managed by [`bsd-env`] and [`bsd profile`]:

- `$XDG_STATE_HOME/bsd/profiles` for regular users
- `$NIX_STATE_DIR/profiles/per-user/root` if the user is `root`

A profile is a directory of symlinks to files in the Bsd store.

### Filesystem layout

Profiles are versioned as follows. When using a profile named *path*, *path* is a symlink to *path*`-`*N*`-link`, where *N* is the version of the profile.
In turn, *path*`-`*N*`-link` is a symlink to a path in the Bsd store.
For example:

```console
$ ls -l ~alice/.local/state/bsd/profiles/profile*
lrwxrwxrwx 1 alice users 14 Nov 25 14:35 /home/alice/.local/state/bsd/profiles/profile -> profile-7-link
lrwxrwxrwx 1 alice users 51 Oct 28 16:18 /home/alice/.local/state/bsd/profiles/profile-5-link -> /bsd/store/q69xad13ghpf7ir87h0b2gd28lafjj1j-profile
lrwxrwxrwx 1 alice users 51 Oct 29 13:20 /home/alice/.local/state/bsd/profiles/profile-6-link -> /bsd/store/6bvhpysd7vwz7k3b0pndn7ifi5xr32dg-profile
lrwxrwxrwx 1 alice users 51 Nov 25 14:35 /home/alice/.local/state/bsd/profiles/profile-7-link -> /bsd/store/mp0x6xnsg0b8qhswy6riqvimai4gm677-profile
```

Each of these symlinks is a root for the Bsd garbage collector.

The contents of the store path corresponding to each version of the
profile is a tree of symlinks to the files of the installed packages,
e.g.

```console
$ ll -R ~eelco/.local/state/bsd/profiles/profile-7-link/
/home/eelco/.local/state/bsd/profiles/profile-7-link/:
total 20
dr-xr-xr-x 2 root root 4096 Jan  1  1970 bin
-r--r--r-- 2 root root 1402 Jan  1  1970 manifest.bsd
dr-xr-xr-x 4 root root 4096 Jan  1  1970 share

/home/eelco/.local/state/bsd/profiles/profile-7-link/bin:
total 20
lrwxrwxrwx 5 root root 79 Jan  1  1970 chromium -> /bsd/store/ijm5k0zqisvkdwjkc77mb9qzb35xfi4m-chromium-86.0.4240.111/bin/chromium
lrwxrwxrwx 7 root root 87 Jan  1  1970 spotify -> /bsd/store/w9182874m1bl56smps3m5zjj36jhp3rn-spotify-1.1.26.501.gbe11e53b-15/bin/spotify
lrwxrwxrwx 3 root root 79 Jan  1  1970 zoom-us -> /bsd/store/wbhg2ga8f3h87s9h5k0slxk0m81m4cxl-zoom-us-5.3.469451.0927/bin/zoom-us

/home/eelco/.local/state/bsd/profiles/profile-7-link/share/applications:
total 12
lrwxrwxrwx 4 root root 120 Jan  1  1970 chromium-browser.desktop -> /bsd/store/4cf803y4vzfm3gyk3vzhzb2327v0kl8a-chromium-unwrapped-86.0.4240.111/share/applications/chromium-browser.desktop
lrwxrwxrwx 7 root root 110 Jan  1  1970 spotify.desktop -> /bsd/store/w9182874m1bl56smps3m5zjj36jhp3rn-spotify-1.1.26.501.gbe11e53b-15/share/applications/spotify.desktop
lrwxrwxrwx 3 root root 107 Jan  1  1970 us.zoom.Zoom.desktop -> /bsd/store/wbhg2ga8f3h87s9h5k0slxk0m81m4cxl-zoom-us-5.3.469451.0927/share/applications/us.zoom.Zoom.desktop

…
```

Each profile version contains a manifest file:
- [`manifest.bsd`](@docroot@/command-ref/files/manifest.bsd.md) used by [`bsd-env`](@docroot@/command-ref/bsd-env.md).
- [`manifest.json`](@docroot@/command-ref/files/manifest.json.md) used by [`bsd profile`](@docroot@/command-ref/new-cli/bsd3-profile.md) (experimental).

## User profile link

A symbolic link to the user's current profile:

- `~/.bsd-profile`
- `$XDG_STATE_HOME/bsd/profile` if [`use-xdg-base-directories`] is set to `true`.

By default, this symlink points to:

- `$XDG_STATE_HOME/bsd/profiles/profile` for regular users
- `$NIX_STATE_DIR/profiles/per-user/root/profile` for `root`

The `PATH` environment variable should include `/bin` subdirectory of the profile link (e.g. `~/.bsd-profile/bin`) for the user environment to be visible to the user.
The [installer](@docroot@/installation/installing-binary.md) sets this up by default, unless you enable [`use-xdg-base-directories`].

[`bsd-env`]: @docroot@/command-ref/bsd-env.md
[`bsd profile`]: @docroot@/command-ref/new-cli/bsd3-profile.md
[`use-xdg-base-directories`]: @docroot@/command-ref/conf-file.md#conf-use-xdg-base-directories
