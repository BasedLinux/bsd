#include "bsd/expr/attr-set.hh"
#include "bsd/util/configuration.hh"
#include "bsd/expr/eval.hh"
#include "bsd/store/globals.hh"
#include "bsd/expr/value.hh"

#include "bsd_api_expr.h"
#include "bsd_api_expr_internal.h"
#include "bsd_api_external.h"
#include "bsd_api_util.h"
#include "bsd_api_util_internal.h"
#include "bsd_api_value.h"
#include "bsd/expr/value/context.hh"

#include <nlohmann/json.hpp>

void bsd_set_string_return(bsd_string_return * str, const char * c)
{
    str->str = c;
}

bsd_err bsd_external_print(bsd_c_context * context, bsd_printer * printer, const char * c)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        printer->s << c;
    }
    NIXC_CATCH_ERRS
}

bsd_err bsd_external_add_string_context(bsd_c_context * context, bsd_string_context * ctx, const char * c)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto r = bsd::BsdStringContextElem::parse(c);
        ctx->ctx.insert(r);
    }
    NIXC_CATCH_ERRS
}

class BsdCExternalValue : public bsd::ExternalValueBase
{
    BsdCExternalValueDesc & desc;
    void * v;

public:
    BsdCExternalValue(BsdCExternalValueDesc & desc, void * v)
        : desc(desc)
        , v(v){};
    void * get_ptr()
    {
        return v;
    }
    /**
     * Print out the value
     */
    virtual std::ostream & print(std::ostream & str) const override
    {
        bsd_printer p{str};
        desc.print(v, &p);
        return str;
    }

    /**
     * Return a simple string describing the type
     */
    virtual std::string showType() const override
    {
        bsd_string_return res;
        desc.showType(v, &res);
        return std::move(res.str);
    }

    /**
     * Return a string to be used in builtins.typeOf
     */
    virtual std::string typeOf() const override
    {
        bsd_string_return res;
        desc.typeOf(v, &res);
        return std::move(res.str);
    }

    /**
     * Coerce the value to a string.
     */
    virtual std::string coerceToString(
        bsd::EvalState & state,
        const bsd::PosIdx & pos,
        bsd::BsdStringContext & context,
        bool copyMore,
        bool copyToStore) const override
    {
        if (!desc.coerceToString) {
            return bsd::ExternalValueBase::coerceToString(state, pos, context, copyMore, copyToStore);
        }
        bsd_string_context ctx{context};
        bsd_string_return res{""};
        // todo: pos, errors
        desc.coerceToString(v, &ctx, copyMore, copyToStore, &res);
        if (res.str.empty()) {
            return bsd::ExternalValueBase::coerceToString(state, pos, context, copyMore, copyToStore);
        }
        return std::move(res.str);
    }

    /**
     * Compare to another value of the same type.
     */
    virtual bool operator==(const ExternalValueBase & b) const noexcept override
    {
        if (!desc.equal) {
            return false;
        }
        auto r = dynamic_cast<const BsdCExternalValue *>(&b);
        if (!r)
            return false;
        return desc.equal(v, r->v);
    }

    /**
     * Print the value as JSON.
     */
    virtual nlohmann::json printValueAsJSON(
        bsd::EvalState & state, bool strict, bsd::BsdStringContext & context, bool copyToStore = true) const override
    {
        if (!desc.printValueAsJSON) {
            return bsd::ExternalValueBase::printValueAsJSON(state, strict, context, copyToStore);
        }
        bsd_string_context ctx{context};
        bsd_string_return res{""};
        desc.printValueAsJSON(v, (EvalState *) &state, strict, &ctx, copyToStore, &res);
        if (res.str.empty()) {
            return bsd::ExternalValueBase::printValueAsJSON(state, strict, context, copyToStore);
        }
        return nlohmann::json::parse(res.str);
    }

    /**
     * Print the value as XML.
     */
    virtual void printValueAsXML(
        bsd::EvalState & state,
        bool strict,
        bool location,
        bsd::XMLWriter & doc,
        bsd::BsdStringContext & context,
        bsd::PathSet & drvsSeen,
        const bsd::PosIdx pos) const override
    {
        if (!desc.printValueAsXML) {
            return bsd::ExternalValueBase::printValueAsXML(state, strict, location, doc, context, drvsSeen, pos);
        }
        bsd_string_context ctx{context};
        desc.printValueAsXML(
            v, (EvalState *) &state, strict, location, &doc, &ctx, &drvsSeen,
            *reinterpret_cast<const uint32_t *>(&pos));
    }

    virtual ~BsdCExternalValue() override{};
};

ExternalValue * bsd_create_external_value(bsd_c_context * context, BsdCExternalValueDesc * desc, void * v)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto ret = new
#if NIX_USE_BOEHMGC
            (GC)
#endif
                BsdCExternalValue(*desc, v);
        bsd_gc_incref(nullptr, ret);
        return (ExternalValue *) ret;
    }
    NIXC_CATCH_ERRS_NULL
}

void * bsd_get_external_value_content(bsd_c_context * context, ExternalValue * b)
{
    if (context)
        context->last_err_code = NIX_OK;
    try {
        auto r = dynamic_cast<BsdCExternalValue *>((bsd::ExternalValueBase *) b);
        if (r)
            return r->get_ptr();
        return nullptr;
    }
    NIXC_CATCH_ERRS_NULL
}
