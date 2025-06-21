#pragma once
///@file

#include "bsd/expr/bsdexpr.hh"
#include "bsd/expr/eval.hh"

#include <string>
#include <map>

namespace bsd {

void printValueAsXML(EvalState & state, bool strict, bool location,
    Value & v, std::ostream & out, BsdStringContext & context, const PosIdx pos);

}
