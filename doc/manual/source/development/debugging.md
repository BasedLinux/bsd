# Debugging Bsd

This section shows how to build and debug Bsd with debug symbols enabled.

Additionally, see [Testing Bsd](./testing.md) for further instructions on how to debug Bsd in the context of a unit test or functional test.

## Building Bsd with Debug Symbols

In the development shell, set the `mesonBuildType` environment variable to `debug` before configuring the build:

```console
[bsd-shell]$ export mesonBuildType=debugoptimized
```

Then, proceed to build Bsd as described in [Building Bsd](./building.md).
This will build Bsd with debug symbols, which are essential for effective debugging.

It is also possible to build without debugging for faster build:

```console
[bsd-shell]$ NIX_HARDENING_ENABLE=$(printLines $NIX_HARDENING_ENABLE | grep -v fortify)
[bsd-shell]$ export mesonBuildType=debug
```

(The first line is needed because `fortify` hardening requires at least some optimization.)

## Debugging the Bsd Binary

Obtain your preferred debugger within the development shell:

```console
[bsd-shell]$ bsd-shell -p gdb
```

On macOS, use `lldb`:

```console
[bsd-shell]$ bsd-shell -p lldb
```

### Launching the Debugger

To debug the Bsd binary, run:

```console
[bsd-shell]$ gdb --args ../outputs/out/bin/bsd
```

On macOS, use `lldb`:

```console
[bsd-shell]$ lldb -- ../outputs/out/bin/bsd
```

### Using the Debugger

Inside the debugger, you can set breakpoints, run the program, and inspect variables.

```gdb
(gdb) break main
(gdb) run <arguments>
```

Refer to the [GDB Documentation](https://www.gnu.org/software/gdb/documentation/) for comprehensive usage instructions.

On macOS, use `lldb`:

```lldb
(lldb) breakpoint set --name main
(lldb) process launch -- <arguments>
```

Refer to the [LLDB Tutorial](https://lldb.llvm.org/use/tutorial.html) for comprehensive usage instructions.
