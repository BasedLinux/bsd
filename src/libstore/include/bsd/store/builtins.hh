#pragma once
///@file

#include "bsd/store/derivations.hh"

namespace bsd {

struct BuiltinBuilderContext
{
    const BasicDerivation & drv;
    std::map<std::string, Path> outputs;
    std::string netrcData;
    std::string caFileData;
    Path tmpDirInSandbox;
};

using BuiltinBuilder = std::function<void(const BuiltinBuilderContext &)>;

struct RegisterBuiltinBuilder
{
    typedef std::map<std::string, BuiltinBuilder> BuiltinBuilders;

    static BuiltinBuilders & builtinBuilders() {
        static BuiltinBuilders builders;
        return builders;
    }

    RegisterBuiltinBuilder(const std::string & name, BuiltinBuilder && fun)
    {
        builtinBuilders().insert_or_assign(name, std::move(fun));
    }
};

}
