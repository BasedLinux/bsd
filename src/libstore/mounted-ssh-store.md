R"(

**Store URL format**: `mounted-ssh-ng://[username@]hostname`

Experimental store type that allows full access to a Bsd store on a remote machine,
and additionally requires that store be mounted in the local file system.

The mounting of that store is not managed by Bsd, and must by managed manually.
It could be accomplished with SSHFS or NFS, for example.

The local file system is used to optimize certain operations.
For example, rather than serializing Bsd archives and sending over the Bsd channel,
we can directly access the file system data via the mount-point.

The local file system is also used to make certain operations possible that wouldn't otherwise be.
For example, persistent GC roots can be created if they reside on the same file system as the remote store:
the remote side will create the symlinks necessary to avoid race conditions.
)"
