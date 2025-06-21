#include "bsd/store/store-api.hh"
#include "bsd/expr/eval.hh"

namespace bsd {

SourcePath EvalState::rootPath(CanonPath path)
{
    return {rootFS, std::move(path)};
}

SourcePath EvalState::rootPath(PathView path)
{
    return {rootFS, CanonPath(absPath(path))};
}

SourcePath EvalState::storePath(const StorePath & path)
{
    return {rootFS, CanonPath{store->printStorePath(path)}};
}

}
