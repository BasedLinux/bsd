#pragma once
///@file

#include "bsd/util/configuration.hh"

namespace bsd {

enum struct EvalProfilerMode { disabled, flamegraph };

template<>
EvalProfilerMode BaseSetting<EvalProfilerMode>::parse(const std::string & str) const;

template<>
std::string BaseSetting<EvalProfilerMode>::to_string() const;

}
