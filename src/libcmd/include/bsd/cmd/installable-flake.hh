#pragma once
///@file

#include "bsd/cmd/common-eval-args.hh"
#include "bsd/cmd/installable-value.hh"

namespace bsd {

/**
 * Extra info about a \ref DerivedPath "derived path" that ultimately
 * come from a Flake.
 *
 * Invariant: every ExtraPathInfo gotten from an InstallableFlake should
 * be possible to downcast to an ExtraPathInfoFlake.
 */
struct ExtraPathInfoFlake : ExtraPathInfoValue
{
    /**
     * Extra struct to get around C++ designated initializer limitations
     */
    struct Flake {
        FlakeRef originalRef;
        FlakeRef lockedRef;
    };

    Flake flake;

    ExtraPathInfoFlake(Value && v, Flake && f)
        : ExtraPathInfoValue(std::move(v)), flake(std::move(f))
    { }
};

struct InstallableFlake : InstallableValue
{
    FlakeRef flakeRef;
    Strings attrPaths;
    Strings prefixes;
    ExtendedOutputsSpec extendedOutputsSpec;
    const flake::LockFlags & lockFlags;
    mutable std::shared_ptr<flake::LockedFlake> _lockedFlake;

    InstallableFlake(
        SourceExprCommand * cmd,
        ref<EvalState> state,
        FlakeRef && flakeRef,
        std::string_view fragment,
        ExtendedOutputsSpec extendedOutputsSpec,
        Strings attrPaths,
        Strings prefixes,
        const flake::LockFlags & lockFlags);

    std::string what() const override { return flakeRef.to_string() + "#" + *attrPaths.begin(); }

    std::vector<std::string> getActualAttrPaths();

    DerivedPathsWithInfo toDerivedPaths() override;

    std::pair<Value *, PosIdx> toValue(EvalState & state) override;

    /**
     * Get a cursor to every attrpath in getActualAttrPaths() that
     * exists. However if none exists, throw an exception.
     */
    std::vector<ref<eval_cache::AttrCursor>>
    getCursors(EvalState & state) override;

    std::shared_ptr<flake::LockedFlake> getLockedFlake() const;

    FlakeRef bsdpkgsFlakeRef() const;
};

/**
 * Default flake ref for referring to Bsdpkgs. For flakes that don't
 * have their own Bsdpkgs input, or other installables.
 *
 * It is a layer violation for Bsd to know about Bsdpkgs; currently just
 * `bsd develop` does. Be wary of using this /
 * `InstallableFlake::bsdpkgsFlakeRef` more places.
 */
static inline FlakeRef defaultBsdpkgsFlakeRef()
{
    return FlakeRef::fromAttrs(fetchSettings, {{"type","indirect"}, {"id", "bsdpkgs"}});
}

ref<eval_cache::EvalCache> openEvalCache(
    EvalState & state,
    std::shared_ptr<flake::LockedFlake> lockedFlake);

}
