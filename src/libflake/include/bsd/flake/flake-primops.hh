#pragma once

#include "bsd/expr/eval.hh"
#include "bsd/flake/settings.hh"

namespace bsd::flake::primops {

/**
 * Returns a `builtins.getFlake` primop with the given bsd::flake::Settings.
 */
bsd::PrimOp getFlake(const Settings & settings);

extern bsd::PrimOp parseFlakeRef;
extern bsd::PrimOp flakeRefToString;

} // namespace bsd::flake
