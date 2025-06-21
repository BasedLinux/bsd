## Default Bsd expression

The source for the [Bsd expressions](@docroot@/glossary.md#gloss-bsd-expression) used by [`bsd-env`] by default:

- `~/.nix-defexpr`
- `$XDG_STATE_HOME/bsd/defexpr` if [`use-xdg-base-directories`] is set to `true`.

It is loaded as follows:

- If the default expression is a file, it is loaded as a Bsd expression.
- If the default expression is a directory containing a `default.nix` file, that `default.nix` file is loaded as a Bsd expression.
- If the default expression is a directory without a `default.nix` file, then its contents (both files and subdirectories) are loaded as Bsd expressions.
  The expressions are combined into a single attribute set, each expression under an attribute with the same name as the original file or subdirectory.
  Subdirectories without a `default.nix` file are traversed recursively in search of more Bsd expressions, but the names of these intermediate directories are not added to the attribute paths of the default Bsd expression.

Then, the resulting expression is interpreted like this:

- If the expression is an attribute set, it is used as the default Bsd expression.
- If the expression is a function, an empty set is passed as argument and the return value is used as the default Bsd expression.

> **Example**
>
> If the default expression contains two files, `foo.nix` and `bar.nix`, then the default Bsd expression will be equivalent to
>
> ```bsd
> {
>   foo = import ~/.nix-defexpr/foo.nix;
>   bar = import ~/.nix-defexpr/bar.nix;
> }
> ```

The file [`manifest.nix`](@docroot@/command-ref/files/manifest.nix.md) is always ignored.

The command [`bsd-channel`] places a symlink to the current user's [channels] in this directory, the [user channel link](#user-channel-link).
This makes all subscribed channels available as attributes in the default expression.

## User channel link

A symlink that ensures that [`bsd-env`] can find the current user's [channels]:

- `~/.nix-defexpr/channels`
- `$XDG_STATE_HOME/defexpr/channels` if [`use-xdg-base-directories`] is set to `true`.

This symlink points to:

- `$XDG_STATE_HOME/profiles/channels` for regular users
- `$NIX_STATE_DIR/profiles/per-user/root/channels` for `root`

In a multi-user installation, you may also have `~/.nix-defexpr/channels_root`, which links to the channels of the root user.

[`bsd-channel`]: @docroot@/command-ref/bsd-channel.md
[`bsd-env`]: @docroot@/command-ref/bsd-env.md
[`use-xdg-base-directories`]: @docroot@/command-ref/conf-file.md#conf-use-xdg-base-directories
[channels]: @docroot@/command-ref/files/channels.md
