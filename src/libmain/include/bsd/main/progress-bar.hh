#pragma once
///@file

#include "bsd/util/logging.hh"

namespace bsd {

std::unique_ptr<Logger> makeProgressBar();

}
