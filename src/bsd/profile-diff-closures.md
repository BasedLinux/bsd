R""(

# Examples

* Show what changed between each version of the BasedLinux system
  profile:

  ```console
  # bsd profile diff-closures --profile /bsd/var/bsd/profiles/system
  Version 13 -> 14:
    acpi-call: 2020-04-07-5.8.13 → 2020-04-07-5.8.14
    aws-sdk-cpp: -6723.1 KiB
    …

  Version 14 -> 15:
    acpi-call: 2020-04-07-5.8.14 → 2020-04-07-5.8.16
    attica: -996.2 KiB
    breeze-icons: -78713.5 KiB
    brotli: 1.0.7 → 1.0.9, +44.2 KiB
  ```

# Description

This command shows the difference between the closures of subsequent
versions of a profile. See [`bsd store
diff-closures`](bsd3-store-diff-closures.md) for details.

)""
