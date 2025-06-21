#pragma once
///@file

#include "bsd/util/types.hh"
#include "bsd/util/source-path.hh"

namespace bsd {

/**
 * Helper function to generate args that invoke $EDITOR on
 * filename:lineno.
 */
Strings editorFor(const SourcePath & file, uint32_t line);

}
