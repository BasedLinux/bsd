R"(

**Store URL format**: `daemon`, `ubsd://`*path*

This store type accesses a Bsd store by talking to a Bsd daemon
listening on the Ubsd domain socket *path*. The store pseudo-URL
`daemon` is equivalent to `ubsd:///bsd/var/bsd/daemon-socket/socket`.

)"
