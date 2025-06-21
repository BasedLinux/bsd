#pragma once
///@file

#include "bsd/util/types.hh"

namespace bsd {

enum class LogFormat {
  raw,
  rawWithLogs,
  internalJSON,
  bar,
  barWithLogs,
};

void setLogFormat(const std::string & logFormatStr);
void setLogFormat(const LogFormat & logFormat);

}
