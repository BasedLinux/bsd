# Architecture

This chapter describes how Bsd works.
It should help users understand why Bsd behaves as it does, and it should help developers understand how to modify Bsd and how to write similar tools.

## Overview

Bsd consists of [hierarchical layers].

[hierarchical layers]: https://en.wikipedia.org/wiki/Multitier_architecture#Layers

The following [concept map] shows its main components (rectangles), the objects they operate on (rounded rectangles), and their interactions (connecting phrases):

[concept map]: https://en.wikipedia.org/wiki/Concept_map

```

   .----------------.
   | Bsd expression |----------.
   '----------------'          |
           |              passed to
           |                   |
+----------|-------------------|--------------------------------+
| Bsd      |                   V                                |
|          |       +------------------------+                   |
|          |       | command line interface |------.            |
|          |       +------------------------+      |            |
|          |                   |                   |            |
|    evaluated by            calls              manages         |
|          |                   |                   |            |
|          |                   V                   |            |
|          |         +--------------------+        |            |
|          '-------->| language evaluator |        |            |
|                    +--------------------+        |            |
|                              |                   |            |
|                           produces               |            |
|                              |                   V            |
| +----------------------------|------------------------------+ |
| | store                      |                              | |
| |            referenced by   V       builds                 | |
| | .-------------.      .------------.      .--------------. | |
| | | build input |----->| build plan |----->| build result | | |
| | '-------------'      '------------'      '--------------' | |
| +-------------------------------------------------|---------+ |
+---------------------------------------------------|-----------+
                                                    |
                                              represented as
                                                    |
                                                    V
                                            .---------------.
                                            |     file      |
                                            '---------------'
```

At the top is the [command line interface](../command-ref/index.md) that drives the underlying layers.

The [Bsd language](../language/index.md) evaluator transforms Bsd expressions into self-contained *build plans*, which are used to derive *build results* from referenced *build inputs*.

The command line interface and Bsd expressions are what users deal with most.

> **Note**
>
> The Bsd language itself does not have a notion of *packages* or *configurations*.
> As far as we are concerned here, the inputs and results of a build plan are just data.

Underlying the command line interface and the Bsd language evaluator is the [Bsd store](../store/index.md), a mechanism to keep track of build plans, data, and references between them.
It can also execute build plans to produce new data, which are made available to the operating system as files.

A build plan itself is a series of *build tasks*, together with their build inputs.

> **Important**
> A build task in Bsd is called [store derivation](@docroot@/glossary.md#gloss-store-derivation).

Each build task has a special build input executed as *build instructions* in order to perform the build.
The result of a build task can be input to another build task.

The following [data flow diagram] shows a build plan for illustration.
Build inputs used as instructions to a build task are marked accordingly:

[data flow diagram]: https://en.wikipedia.org/wiki/Data-flow_diagram

```
+--------------------------------------------------------------------+
| build plan                                                         |
|                                                                    |
| .-------------.                                                    |
| | build input |---------.                                          |
| '-------------'         |                                          |
|                    instructions                                    |
|                         |                                          |
|                         v                                          |
| .-------------.    .----------.                                    |
| | build input |-->( build task )-------.                           |
| '-------------'    '----------'        |                           |
|                                  instructions                      |
|                                        |                           |
|                                        v                           |
| .-------------.                  .----------.     .--------------. |
| | build input |---------.       ( build task )--->| build result | |
| '-------------'         |        '----------'     '--------------' |
|                    instructions        ^                           |
|                         |              |                           |
|                         v              |                           |
| .-------------.    .----------.        |                           |
| | build input |-->( build task )-------'                           |
| '-------------'    '----------'                                    |
|                         ^                                          |
|                         |                                          |
|                         |                                          |
| .-------------.         |                                          |
| | build input |---------'                                          |
| '-------------'                                                    |
|                                                                    |
+--------------------------------------------------------------------+
```

