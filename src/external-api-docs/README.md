# Getting started

> **Warning** These bindings are **experimental**, which means they can change
> at any time or be removed outright; nevertheless the plan is to provide a
> stable external C API to the Bsd language and the Bsd store.

The language library allows evaluating Bsd expressions and interacting with Bsd
language values. The Bsd store API is still rudimentary, and only allows
initialising and connecting to a store for the Bsd language evaluator to
interact with.

Currently there are two ways to interface with the Bsd language evaluator
programmatically:

1. Embedding the evaluator
2. Writing language plug-ins

Embedding means you link the Bsd C libraries in your program and use them from
there. Adding a plug-in means you make a library that gets loaded by the Bsd
language evaluator, specified through a configuration option.

Many of the components and mechanisms involved are not yet documented, therefore
please refer to the [Bsd source code](https://github.com/BasedLinux/bsd/) for
details. Additions to in-code documentation and the reference manual are highly
appreciated.

The following examples, for simplicity, don't include error handling. See the
[Handling errors](@ref errors) section for more information.

# Embedding the Bsd Evaluator{#bsd_evaluator_example}

In this example we programmatically start the Bsd language evaluator with a
dummy store (that has no store paths and cannot be written to), and evaluate the
Bsd expression `builtins.bsdVersion`.

**main.c:**

```C
#include <bsd_api_util.h>
#include <bsd_api_expr.h>
#include <bsd_api_value.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// NOTE: This example lacks all error handling. Production code must check for
// errors, as some return values will be undefined.

void my_get_string_cb(const char * start, unsigned int n, void * user_data)
{
    *((char **) user_data) = strdup(start);
}

int main()
{
    bsd_libexpr_init(NULL);

    Store * store = bsd_store_open(NULL, "dummy://", NULL);
    EvalState * state = bsd_state_create(NULL, NULL, store); // empty search path (NIX_PATH)
    Value * value = bsd_alloc_value(NULL, state);

    bsd_expr_eval_from_string(NULL, state, "builtins.bsdVersion", ".", value);
    bsd_value_force(NULL, state, value);

    char * version;
    bsd_get_string(NULL, value, my_get_string_cb, &version);
    printf("Bsd version: %s\n", version);

    free(version);
    bsd_gc_decref(NULL, value);
    bsd_state_free(state);
    bsd_store_free(store);
    return 0;
}
```

**Usage:**

```ShellSession
$ gcc main.c $(pkg-config bsd-expr-c --libs --cflags) -o main
$ ./main
Bsd version: 2.17
```

# Writing a Bsd language plug-in

In this example we add a custom primitive operation (_primop_) to `builtins`. It
will increment the argument if it is an integer and throw an error otherwise.

**plugin.c:**

```C
#include <bsd_api_util.h>
#include <bsd_api_expr.h>
#include <bsd_api_value.h>

void increment(void* user_data, bsd_c_context* ctx, EvalState* state, Value** args, Value* v) {
    bsd_value_force(NULL, state, args[0]);
    if (bsd_get_type(NULL, args[0]) == NIX_TYPE_INT) {
      bsd_init_int(NULL, v, bsd_get_int(NULL, args[0]) + 1);
    } else {
      bsd_set_err_msg(ctx, NIX_ERR_UNKNOWN, "First argument should be an integer.");
    }
}

void bsd_plugin_entry() {
  const char* args[] = {"n", NULL};
  PrimOp *p = bsd_alloc_primop(NULL, increment, 1, "increment", args, "Example custom built-in function: increments an integer", NULL);
  bsd_register_primop(NULL, p);
  bsd_gc_decref(NULL, p);
}
```

**Usage:**

```ShellSession
$ gcc plugin.c $(pkg-config bsd-expr-c --libs --cflags) -shared -o plugin.so
$ bsd --plugin-files ./plugin.so repl
bsd-repl> builtins.increment 1
2
```
