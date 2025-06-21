# Name

`bsd-store` - manipulate or query the Bsd store

# Synopsis

`bsd-store` *operation* [*options…*] [*arguments…*]
  [`--option` *name* *value*]
  [`--add-root` *path*]

# Description

The command `bsd-store` performs primitive operations on the Bsd store.
You generally do not need to run this command manually.

`bsd-store` takes exactly one *operation* flag which indicates the subcommand to be performed. The following operations are available:

- [`--realise`](./bsd-store/realise.md)
- [`--serve`](./bsd-store/serve.md)
- [`--gc`](./bsd-store/gc.md)
- [`--delete`](./bsd-store/delete.md)
- [`--query`](./bsd-store/query.md)
- [`--add`](./bsd-store/add.md)
- [`--add-fixed`](./bsd-store/add-fixed.md)
- [`--verify`](./bsd-store/verify.md)
- [`--verify-path`](./bsd-store/verify-path.md)
- [`--repair-path`](./bsd-store/repair-path.md)
- [`--dump`](./bsd-store/dump.md)
- [`--restore`](./bsd-store/restore.md)
- [`--export`](./bsd-store/export.md)
- [`--import`](./bsd-store/import.md)
- [`--optimise`](./bsd-store/optimise.md)
- [`--read-log`](./bsd-store/read-log.md)
- [`--dump-db`](./bsd-store/dump-db.md)
- [`--load-db`](./bsd-store/load-db.md)
- [`--print-env`](./bsd-store/print-env.md)
- [`--generate-binary-cache-key`](./bsd-store/generate-binary-cache-key.md)

These pages can be viewed offline:

- `man bsd-store-<operation>`.

  Example: `man bsd-store-realise`

- `bsd-store --help --<operation>`

  Example: `bsd-store --help --realise`
