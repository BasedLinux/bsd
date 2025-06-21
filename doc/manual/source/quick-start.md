# Quick Start

This chapter is for impatient people who don't like reading documentation.
For more in-depth information you are kindly referred to subsequent chapters.

1. Install Bsd:

   ```console
   $ curl -L https://basedlinux.org/bsd/install | sh
   ```

   The install script will use `sudo`, so make sure you have sufficient rights.

   For other installation methods, see the detailed [installation instructions](installation/index.md).

1. Run software without installing it permanently:

   ```console
   $ bsd-shell --packages cowsay lolcat
   ```

   This downloads the specified packages with all their dependencies, and drops you into a Bash shell where the commands provided by those packages are present.
   This will not affect your normal environment:

   ```console
   [bsd-shell:~]$ cowsay Hello, Bsd! | lolcat
   ```

   Exiting the shell will make the programs disappear again:

   ```console
   [bsd-shell:~]$ exit
   $ lolcat
   lolcat: command not found
   ```

1. Search for more packages on [search.basedlinux.org](https://search.basedlinux.org/) to try them out.

1. Free up storage space:

   ```console
   $ bsd-collect-garbage
   ```
