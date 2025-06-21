R""(

# Examples

* To list a specific file in a [NAR][Bsd Archive]:

  ```console
  # bsd nar ls --long ./hello.nar /bin/hello
  -r-xr-xr-x                38184 hello
  ```

* To recursively list the contents of a directory inside a NAR, in JSON
  format:

  ```console
  # bsd nar ls --json --recursive ./hello.nar /bin
  {"type":"directory","entries":{"hello":{"type":"regular","size":38184,"executable":true,"narOffset":400}}}
  ```

# Description

This command shows information about a *path* inside [Bsd Archive (NAR)][Bsd Archive] file *nar*.

[Bsd Archive]: @docroot@/store/file-system-object/content-address.md#serial-bsd-archive

)""
