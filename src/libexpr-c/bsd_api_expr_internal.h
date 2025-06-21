#ifndef NIX_API_EXPR_INTERNAL_H
#define NIX_API_EXPR_INTERNAL_H

#include "bsd/fetchers/fetch-settings.hh"
#include "bsd/expr/eval.hh"
#include "bsd/expr/eval-settings.hh"
#include "bsd/expr/attr-set.hh"
#include "bsd_api_value.h"
#include "bsd/expr/search-path.hh"

struct bsd_eval_state_builder
{
    bsd::ref<bsd::Store> store;
    bsd::EvalSettings settings;
    bsd::fetchers::Settings fetchSettings;
    bsd::LookupPath lookupPath;
    // TODO: make an EvalSettings setting own this instead?
    bool readOnlyMode;
};

struct EvalState
{
    bsd::fetchers::Settings fetchSettings;
    bsd::EvalSettings settings;
    bsd::EvalState state;
};

struct BindingsBuilder
{
    bsd::BindingsBuilder builder;
};

struct ListBuilder
{
    bsd::ListBuilder builder;
};

struct bsd_value
{
    bsd::Value value;
};

struct bsd_string_return
{
    std::string str;
};

struct bsd_printer
{
    std::ostream & s;
};

struct bsd_string_context
{
    bsd::BsdStringContext & ctx;
};

struct bsd_realised_string
{
    std::string str;
    std::vector<StorePath> storePaths;
};

#endif // NIX_API_EXPR_INTERNAL_H
