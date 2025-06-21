#include "bsd/cmd/command-installable-value.hh"

namespace bsd {

void InstallableValueCommand::run(ref<Store> store, ref<Installable> installable)
{
    auto installableValue = InstallableValue::require(installable);
    run(store, installableValue);
}

}
