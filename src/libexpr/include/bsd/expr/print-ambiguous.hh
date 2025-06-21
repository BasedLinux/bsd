#pragma once

#include "bsd/expr/value.hh"
#include "bsd/expr/symbol-table.hh"

namespace bsd {

/**
 * Print a value in the deprecated format used by `bsd-instantiate --eval` and
 * `bsd-env` (for manifests).
 *
 * This output can't be changed because it's part of the `bsd-instantiate` API,
 * but it produces ambiguous output; unevaluated thunks and lambdas (and a few
 * other types) are printed as Bsd path syntax like `<CODE>`.
 *
 * See: https://github.com/BasedLinux/bsd/issues/9730
 */
void printAmbiguous(
    Value &v,
    const SymbolTable &symbols,
    std::ostream &str,
    std::set<const void *> *seen,
    int depth);

}
