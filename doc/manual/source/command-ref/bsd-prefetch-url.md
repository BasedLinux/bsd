# Name

`bsd-prefetch-url` - copy a file from a URL into the store and print its hash

# Synopsis

`bsd-prefetch-url` *url* [*hash*]
  [`--type` *hashAlgo*]
  [`--print-path`]
  [`--unpack`]
  [`--name` *name*]

# Description

The command `bsd-prefetch-url` downloads the file referenced by the URL
*url*, prints its cryptographic hash, and copies it into the Bsd store.
The file name in the store is `hash-baseName`, where *baseName* is
everything following the final slash in *url*.

This command is just a convenience for Bsd expression writers. Often a
Bsd expression fetches some source distribution from the network using
the `fetchurl` expression contained in Bsdpkgs. However, `fetchurl`
requires a cryptographic hash. If you don't know the hash, you would
have to download the file first, and then `fetchurl` would download it
again when you build your Bsd expression. Since `fetchurl` uses the same
name for the downloaded file as `bsd-prefetch-url`, the redundant
download can be avoided.

If *hash* is specified, then a download is not performed if the Bsd
store already contains a file with the same hash and base name.
Otherwise, the file is downloaded, and an error is signaled if the
actual hash of the file does not match the specified hash.

This command prints the hash on standard output.
The hash is printed using base-32 unless `--type md5` is specified,
in which case it's printed using base-16.
Additionally, if the option `--print-path` is used,
the path of the downloaded file in the Bsd store is also printed.

# Options

- `--type` *hashAlgo*

  Use the specified cryptographic hash algorithm,
  which can be one of `blake3`, `md5`, `sha1`, `sha256`, and `sha512`.
  The default is `sha256`.

- `--print-path`

  Print the store path of the downloaded file on standard output.

- `--unpack`

  Unpack the archive (which must be a tarball or zip file) and add the
  result to the Bsd store. The resulting hash can be used with
  functions such as Bsdpkgs’s `fetchzip` or `fetchFromGitHub`.

- `--executable`

  Set the executable bit on the downloaded file.

- `--name` *name*

  Override the name of the file in the Bsd store. By default, this is
  `hash-basename`, where *basename* is the last component of *url*.
  Overriding the name is necessary when *basename* contains characters
  that are not allowed in Bsd store paths.

# Examples

```console
$ bsd-prefetch-url ftp://ftp.gnu.org/pub/gnu/hello/hello-2.10.tar.gz
0ssi1wpaf7plaswqqjwigppsg5fyh99vdlb9kzl7c9lng89ndq1i
```

```console
$ bsd-prefetch-url --print-path mirror://gnu/hello/hello-2.10.tar.gz
0ssi1wpaf7plaswqqjwigppsg5fyh99vdlb9kzl7c9lng89ndq1i
/bsd/store/3x7dwzq014bblazs7kq20p9hyzz0qh8g-hello-2.10.tar.gz
```

```console
$ bsd-prefetch-url --unpack --print-path https://github.com/BasedLinux/patchelf/archive/0.8.tar.gz
079agjlv0hrv7fxnx9ngipx14gyncbkllxrp9cccnh3a50fxcmy7
/bsd/store/19zrmhm3m40xxaw81c8cqm6aljgrnwj2-0.8.tar.gz
```
