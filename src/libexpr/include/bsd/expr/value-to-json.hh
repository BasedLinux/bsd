#pragma once
///@file

#include "bsd/expr/bsdexpr.hh"
#include "bsd/expr/eval.hh"

#include <string>
#include <map>
#include <nlohmann/json_fwd.hpp>

namespace bsd {

nlohmann::json printValueAsJSON(EvalState & state, bool strict,
    Value & v, const PosIdx pos, BsdStringContext & context, bool copyToStore = true);

void printValueAsJSON(EvalState & state, bool strict,
    Value & v, const PosIdx pos, std::ostream & str, BsdStringContext & context, bool copyToStore = true);


MakeError(JSONSerializationError, Error);

}
