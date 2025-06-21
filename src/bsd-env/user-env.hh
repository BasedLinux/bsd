#pragma once
///@file

#include "bsd/expr/get-drvs.hh"

namespace bsd {

PackageInfos queryInstalled(EvalState & state, const Path & userEnv);

bool createUserEnv(EvalState & state, PackageInfos & elems,
    const Path & profile, bool keepDerivations,
    const std::string & lockToken);

}
