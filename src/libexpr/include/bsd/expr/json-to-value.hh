#pragma once
///@file

#include "bsd/util/error.hh"

#include <string>

namespace bsd {

class EvalState;
struct Value;

MakeError(JSONParseError, Error);

void parseJSON(EvalState & state, const std::string_view & s, Value & v);

}
