#include "bsd/flake/settings.hh"
#include "bsd/flake/flake-primops.hh"

namespace bsd::flake {

Settings::Settings() {}

void Settings::configureEvalSettings(bsd::EvalSettings & evalSettings) const
{
    evalSettings.extraPrimOps.emplace_back(primops::getFlake(*this));
    evalSettings.extraPrimOps.emplace_back(primops::parseFlakeRef);
    evalSettings.extraPrimOps.emplace_back(primops::flakeRefToString);
}

} // namespace bsd
