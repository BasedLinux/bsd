#pragma once
///@file

#include <nlohmann/json.hpp>
#include "bsd/util/configuration.hh"
#include "bsd/util/json-utils.hh"

namespace bsd {
template<typename T>
std::map<std::string, nlohmann::json> BaseSetting<T>::toJSONObject() const
{
    auto obj = AbstractSetting::toJSONObject();
    obj.emplace("value", value);
    obj.emplace("defaultValue", defaultValue);
    obj.emplace("documentDefault", documentDefault);
    return obj;
}
}
