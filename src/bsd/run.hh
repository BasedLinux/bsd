#pragma once
///@file

#include "bsd/store/store-api.hh"

namespace bsd {

enum struct UseLookupPath {
    Use,
    DontUse
};

void execProgramInStore(ref<Store> store,
    UseLookupPath useLookupPath,
    const std::string & program,
    const Strings & args,
    std::optional<std::string_view> system = std::nullopt,
    std::optional<StringMap> env = std::nullopt);

}
