# Release 0.9.2 (2005-09-21)

This bug fix release fixes two problems on Mac OS X:

  - If Bsd was linked against statically linked versions of the ATerm or
    Berkeley DB library, there would be dynamic link errors at runtime.

  - `bsd-pull` and `bsd-push` intermittently failed due to race
    conditions involving pipes and child processes with error messages
    such as `open2: open(GLOB(0x180b2e4), >&=9) failed: Bad
            file descriptor at /bsd/bin/bsd-pull line 77` (issue `NIX-14`).
