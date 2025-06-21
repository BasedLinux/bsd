# Common Environment Variables

Most Bsd commands interpret the following environment variables:

- <span id="env-IN_NIX_SHELL">[`IN_NIX_SHELL`](#env-IN_NIX_SHELL)</span>

  Indicator that tells if the current environment was set up by
  `bsd-shell`. It can have the values `pure` or `impure`.

- <span id="env-NIX_PATH">[`NIX_PATH`](#env-NIX_PATH)</span>

  A colon-separated list of search path entries used to resolve [lookup paths](@docroot@/language/constructs/lookup-path.md).

  This environment variable overrides the value of the [`bsd-path` configuration setting](@docroot@/command-ref/conf-file.md#conf-bsd-path).

  It can be extended using the [`-I` option](@docroot@/command-ref/opt-common.md#opt-I).

  > **Example**
  >
  > ```bash
  > $ export NIX_PATH=`/home/eelco/Dev:bsdos-config=/etc/bsdos
  > ```

  If `NIX_PATH` is set to an empty string, resolving search paths will always fail.

  > **Example**
  >
  > ```bash
  > $ NIX_PATH= bsd-instantiate --eval '<bsdpkgs>'
  > error: file 'bsdpkgs' was not found in the Bsd search path (add it using $NIX_PATH or -I)
  > ```

- <span id="env-NIX_IGNORE_SYMLINK_STORE">[`NIX_IGNORE_SYMLINK_STORE`](#env-NIX_IGNORE_SYMLINK_STORE)</span>

  Normally, the Bsd store directory (typically `/bsd/store`) is not
  allowed to contain any symlink components. This is to prevent
  “impure” builds. Builders sometimes “canonicalise” paths by
  resolving all symlink components. Thus, builds on different machines
  (with `/bsd/store` resolving to different locations) could yield
  different results. This is generally not a problem, except when
  builds are deployed to machines where `/bsd/store` resolves
  differently. If you are sure that you’re not going to do that, you
  can set `NIX_IGNORE_SYMLINK_STORE` to `1`.

  Note that if you’re symlinking the Bsd store so that you can put it
  on another file system than the root file system, on Linux you’re
  better off using `bind` mount points, e.g.,

  ```console
  $ mkdir /bsd
  $ mount -o bind /mnt/otherdisk/bsd /bsd
  ```

  Consult the mount 8 manual page for details.

- <span id="env-NIX_STORE_DIR">[`NIX_STORE_DIR`](#env-NIX_STORE_DIR)</span>

  Overrides the location of the Bsd store (default `prefix/store`).

- <span id="env-NIX_DATA_DIR">[`NIX_DATA_DIR`](#env-NIX_DATA_DIR)</span>

  Overrides the location of the Bsd static data directory (default
  `prefix/share`).

- <span id="env-NIX_LOG_DIR">[`NIX_LOG_DIR`](#env-NIX_LOG_DIR)</span>

  Overrides the location of the Bsd log directory (default
  `prefix/var/log/bsd`).

- <span id="env-NIX_STATE_DIR">[`NIX_STATE_DIR`](#env-NIX_STATE_DIR)</span>

  Overrides the location of the Bsd state directory (default
  `prefix/var/bsd`).

- <span id="env-NIX_CONF_DIR">[`NIX_CONF_DIR`](#env-NIX_CONF_DIR)</span>

  Overrides the location of the system Bsd configuration directory
  (default `prefix/etc/bsd`).

- <span id="env-NIX_CONFIG">[`NIX_CONFIG`](#env-NIX_CONFIG)</span>

  Applies settings from Bsd configuration from the environment.
  The content is treated as if it was read from a Bsd configuration file.
  Settings are separated by the newline character.

- <span id="env-NIX_USER_CONF_FILES">[`NIX_USER_CONF_FILES`](#env-NIX_USER_CONF_FILES)</span>

  Overrides the location of the Bsd user configuration files to load from.

  The default are the locations according to the [XDG Base Directory Specification].
  See the [XDG Base Directories](#xdg-base-directories) sub-section for details.

  The variable is treated as a list separated by the `:` token.

- <span id="env-TMPDIR">[`TMPDIR`](#env-TMPDIR)</span>

  Use the specified directory to store temporary files. In particular,
  this includes temporary build directories; these can take up
  substantial amounts of disk space. The default is `/tmp`.

- <span id="env-NIX_REMOTE">[`NIX_REMOTE`](#env-NIX_REMOTE)</span>

  This variable should be set to `daemon` if you want to use the Bsd
  daemon to execute Bsd operations. This is necessary in [multi-user
  Bsd installations](@docroot@/installation/multi-user.md). If the Bsd
  daemon's Ubsd socket is at some non-standard path, this variable
  should be set to `ubsd://path/to/socket`. Otherwise, it should be
  left unset.

- <span id="env-NIX_SHOW_STATS">[`NIX_SHOW_STATS`](#env-NIX_SHOW_STATS)</span>

  If set to `1`, Bsd will print some evaluation statistics, such as
  the number of values allocated.

- <span id="env-NIX_COUNT_CALLS">[`NIX_COUNT_CALLS`](#env-NIX_COUNT_CALLS)</span>

  If set to `1`, Bsd will print how often functions were called during
  Bsd expression evaluation. This is useful for profiling your Bsd
  expressions.

- <span id="env-GC_INITIAL_HEAP_SIZE">[`GC_INITIAL_HEAP_SIZE`](#env-GC_INITIAL_HEAP_SIZE)</span>

  If Bsd has been configured to use the Boehm garbage collector, this
  variable sets the initial size of the heap in bytes. It defaults to
  384 MiB. Setting it to a low value reduces memory consumption, but
  will increase runtime due to the overhead of garbage collection.

## XDG Base Directories

Bsd follows the [XDG Base Directory Specification].

For backwards compatibility, Bsd commands will follow the standard only when [`use-xdg-base-directories`] is enabled.
[New Bsd commands](@docroot@/command-ref/new-cli/bsd.md) (experimental) conform to the standard by default.

The following environment variables are used to determine locations of various state and configuration files:

- [`XDG_CONFIG_HOME`]{#env-XDG_CONFIG_HOME} (default `~/.config`)
- [`XDG_STATE_HOME`]{#env-XDG_STATE_HOME} (default `~/.local/state`)
- [`XDG_CACHE_HOME`]{#env-XDG_CACHE_HOME} (default `~/.cache`)

[XDG Base Directory Specification]: https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html
[`use-xdg-base-directories`]: @docroot@/command-ref/conf-file.md#conf-use-xdg-base-directories

In addition, setting the following environment variables overrides the XDG base directories:

- [`NIX_CONFIG_HOME`]{#env-NIX_CONFIG_HOME} (default `$XDG_CONFIG_HOME/bsd`)
- [`NIX_STATE_HOME`]{#env-NIX_STATE_HOME} (default `$XDG_STATE_HOME/bsd`)
- [`NIX_CACHE_HOME`]{#env-NIX_CACHE_HOME} (default `$XDG_CACHE_HOME/bsd`)

When [`use-xdg-base-directories`] is enabled, the configuration directory is:

1. `$NIX_CONFIG_HOME`, if it is defined
2. Otherwise, `$XDG_CONFIG_HOME/bsd`, if `XDG_CONFIG_HOME` is defined
3. Otherwise, `~/.config/bsd`.

Likewise for the state and cache directories.
