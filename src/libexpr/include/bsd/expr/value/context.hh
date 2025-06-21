#pragma once
///@file

#include "bsd/util/comparator.hh"
#include "bsd/store/derived-path.hh"
#include "bsd/util/variant-wrapper.hh"

#include <nlohmann/json_fwd.hpp>

namespace bsd {

class BadBsdStringContextElem : public Error
{
public:
    std::string_view raw;

    template<typename... Args>
    BadBsdStringContextElem(std::string_view raw_, const Args & ... args)
        : Error("")
    {
        raw = raw_;
        auto hf = HintFmt(args...);
        err.msg = HintFmt("Bad String Context element: %1%: %2%", Uncolored(hf.str()), raw);
    }
};

struct BsdStringContextElem {
    /**
     * Plain opaque path to some store object.
     *
     * Encoded as just the path: `<path>`.
     */
    using Opaque = SingleDerivedPath::Opaque;

    /**
     * Path to a derivation and its entire build closure.
     *
     * The path doesn't just refer to derivation itself and its closure, but
     * also all outputs of all derivations in that closure (including the
     * root derivation).
     *
     * Encoded in the form `=<drvPath>`.
     */
    struct DrvDeep {
        StorePath drvPath;

        GENERATE_CMP(DrvDeep, me->drvPath);
    };

    /**
     * Derivation output.
     *
     * Encoded in the form `!<output>!<drvPath>`.
     */
    using Built = SingleDerivedPath::Built;

    using Raw = std::variant<
        Opaque,
        DrvDeep,
        Built
    >;

    Raw raw;

    GENERATE_CMP(BsdStringContextElem, me->raw);

    MAKE_WRAPPER_CONSTRUCTOR(BsdStringContextElem);

    /**
     * Decode a context string, one of:
     * - `<path>`
     * - `=<path>`
     * - `!<name>!<path>`
     *
     * @param xpSettings Stop-gap to avoid globals during unit tests.
     */
    static BsdStringContextElem parse(
        std::string_view s,
        const ExperimentalFeatureSettings & xpSettings = experimentalFeatureSettings);
    std::string to_string() const;
};

typedef std::set<BsdStringContextElem> BsdStringContext;

}
