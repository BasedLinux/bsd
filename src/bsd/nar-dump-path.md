R""(

# Examples

* To serialise directory `foo` as a [Bsd Archive (NAR)][Bsd Archive]:

  ```console
  # bsd nar pack ./foo > foo.nar
  ```

# Description

This command generates a [Bsd Archive (NAR)][Bsd Archive] file containing the serialisation of
*path*, which must contain only regular files, directories and
symbolic links. The NAR is written to standard output.

[Bsd Archive]: @docroot@/store/file-system-object/content-address.md#serial-bsd-archive

)""
