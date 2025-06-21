R""(

# Examples

* Run the daemon:

  ```console
  # bsd daemon
  ```

* Run the daemon and listen on standard I/O instead of binding to a UNIX socket:

  ```console
  # bsd daemon --stdio
  ```

* Run the daemon and force all connections to be trusted:

  ```console
  # bsd daemon --force-trusted
  ```

* Run the daemon and force all connections to be untrusted:

  ```console
  # bsd daemon --force-untrusted
  ```

* Run the daemon, listen on standard I/O, and force all connections to use Bsd's default trust:

  ```console
  # bsd daemon --stdio --default-trust
  ```

# Description

This command runs the Bsd daemon, which is a required component in
multi-user Bsd installations. It runs build tasks and other
operations on the Bsd store on behalf of non-root users. Usually you
don't run the daemon directly; instead it's managed by a service
management framework such as `systemd` on Linux, or `launchctl` on Darwin.

Note that this daemon does not fork into the background.

)""
