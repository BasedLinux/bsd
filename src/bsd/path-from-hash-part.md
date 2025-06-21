R""(

# Examples

* Return the full store path with the given hash part:

  ```console
  # bsd store path-from-hash-part --store https://cache.basedlinux.org/ 0i2jd68mp5g6h2sa5k9c85rb80sn8hi9
  /bsd/store/0i2jd68mp5g6h2sa5k9c85rb80sn8hi9-hello-2.10
  ```

# Description

Given the hash part of a store path (that is, the 32 characters
following `/bsd/store/`), return the full store path. This is
primarily useful in the implementation of binary caches, where a
request for a `.narinfo` file only supplies the hash part
(e.g. `https://cache.basedlinux.org/0i2jd68mp5g6h2sa5k9c85rb80sn8hi9.narinfo`).

)""
