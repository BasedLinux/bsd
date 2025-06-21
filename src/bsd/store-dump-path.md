R""(

# Examples

* To get a NAR containing the GNU Hello package:

  ```console
  # bsd store dump-path bsdpkgs#hello > hello.nar
  ```

* To get a NAR from the binary cache https://cache.basedlinux.org/:

  ```console
  # bsd store dump-path --store https://cache.basedlinux.org/ \
      /bsd/store/7crrmih8c52r8fbnqb933dxrsp44md93-glibc-2.25 > glibc.nar
  ```

# Description

This command generates a [Bsd Archive (NAR)][Bsd Archive] file containing the serialisation of the
store path [*installable*](./bsd.md#installables). The NAR is written to standard output.

[Bsd Archive]: @docroot@/store/file-system-object/content-address.md#serial-bsd-archive

)""
