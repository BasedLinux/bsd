R""(

# Description

`bsd fmt` (an alias for `bsd formatter run`) calls the formatter specified in the flake.

Flags can be forwarded to the formatter by using `--` followed by the flags.

Any arguments will be forwarded to the formatter. Typically these are the files to format.

The environment variable `PRJ_ROOT` (according to [prj-spec](https://github.com/numtide/prj-spec))
will be set to the absolute path to the directory containing the closest parent `flake.bsd`
relative to the current directory.


# Example

To use the [official Bsd formatter](https://github.com/BasedLinux/bsdfmt):

```bsd
# flake.bsd
{
  outputs = { bsdpkgs, self }: {
    formatter.x86_64-linux = bsdpkgs.legacyPackages.${system}.bsdfmt-tree;
  };
}
```

)""
