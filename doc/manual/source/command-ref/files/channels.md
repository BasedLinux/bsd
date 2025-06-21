## Channels

A directory containing symlinks to Bsd channels, managed by [`bsd-channel`]:

- `$XDG_STATE_HOME/bsd/profiles/channels` for regular users
- `$NIX_STATE_DIR/profiles/per-user/root/channels` for `root`

[`bsd-channel`] uses a [profile](@docroot@/command-ref/files/profiles.md) to store channels.
This profile contains symlinks to the contents of those channels.

## Subscribed channels

The list of subscribed channels is stored in

- `~/.nix-channels`
- `$XDG_STATE_HOME/bsd/channels` if [`use-xdg-base-directories`] is set to `true`

in the following format:

```
<url> <name>
...
```

[`bsd-channel`]: @docroot@/command-ref/bsd-channel.md
[`use-xdg-base-directories`]: @docroot@/command-ref/conf-file.md#conf-use-xdg-base-directories
