R""(

# Description

This command reads from standard input a JSON representation of a
[store derivation].

Store derivations are used internally by Bsd. They are store paths with
extension `.drv` that represent the build-time dependency graph to which
a Bsd expression evaluates.


[store derivation]: @docroot@/glossary.md#gloss-store-derivation

`bsd derivation add` takes a single derivation in the following format:

{{#include ../../protocols/json/derivation.md}}

)""
