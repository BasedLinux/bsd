R""(

# Examples

* Create a flake using the default template in the directory `hello`:

  ```console
  # bsd flake new hello
  ```

* List available templates:

  ```console
  # bsd flake show templates
  ```

* Create a flake from a specific template in the directory `hello`:

  ```console
  # bsd flake new hello -t templates#trivial
  ```

# Description

This command creates a flake in the directory `dest-dir`, which must
not already exist. It's equivalent to:

```console
# mkdir dest-dir
# cd dest-dir
# bsd flake init
```

)""
