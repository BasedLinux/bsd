# Single-User Mode

In single-user mode, all Bsd operations that access the database in
`prefix/var/bsd/db` or modify the Bsd store in `prefix/store` must be
performed under the user ID that owns those directories. This is
typically root. (If you install from RPM packages, that’s in fact the
default ownership.) However, on single-user machines, it is often
convenient to `chown` those directories to your normal user account so
that you don’t have to `su` to root all the time.
