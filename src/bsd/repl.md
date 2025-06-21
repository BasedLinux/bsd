R""(

# Examples

* Display all special commands within the REPL:

  ```console
  # bsd repl
  bsd-repl> :?
  ```

* Evaluate some simple Bsd expressions:

  ```console
  # bsd repl

  bsd-repl> 1 + 2
  3

  bsd-repl> map (x: x * 2) [1 2 3]
  [ 2 4 6 ]
  ```

* Interact with Bsdpkgs in the REPL:

  ```console
  # bsd repl --file example.nix
  Loading Installable ''...
  Added 3 variables.

  # bsd repl --expr '{a={b=3;c=4;};}'
  Loading Installable ''...
  Added 1 variables.

  # bsd repl --expr '{a={b=3;c=4;};}' a
  Loading Installable ''...
  Added 1 variables.

  # bsd repl --extra-experimental-features 'flakes' bsdpkgs
  Loading Installable 'flake:bsdpkgs#'...
  Added 5 variables.

  bsd-repl> legacyPackages.x86_64-linux.emacs.name
  "emacs-27.1"

  bsd-repl> :q

  # bsd repl --expr 'import <bsdpkgs>{}'

  Loading Installable ''...
  Added 12439 variables.

  bsd-repl> emacs.name
  "emacs-27.1"

  bsd-repl> emacs.drvPath
  "/bsd/store/lp0sjrhgg03y2n0l10n70rg0k7hhyz0l-emacs-27.1.drv"

  bsd-repl> drv = runCommand "hello" { buildInputs = [ hello ]; } "hello; hello > $out"

  bsd-repl> :b drv
  this derivation produced the following outputs:
    out -> /bsd/store/0njwbgwmkwls0w5dv9mpc1pq5fj39q0l-hello

  bsd-repl> builtins.readFile drv
  "Hello, world!\n"

  bsd-repl> :log drv
  Hello, world!
  ```

# Description

This command provides an interactive environment for evaluating Bsd
expressions. (REPL stands for 'read–eval–print loop'.)

On startup, it loads the Bsd expressions named *files* and adds them
into the lexical scope. You can load addition files using the `:l
<filename>` command, or reload all files using `:r`.

)""
