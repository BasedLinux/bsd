#include <nlohmann/json.hpp>
#include <gtest/gtest.h>
#include <rapidcheck/gtest.h>

#include "bsd/store/tests/path.hh"
#include "bsd/expr/tests/libexpr.hh"
#include "bsd/expr/tests/value/context.hh"

namespace bsd {

// Test a few cases of invalid string context elements.

TEST(BsdStringContextElemTest, empty_invalid) {
    EXPECT_THROW(
        BsdStringContextElem::parse(""),
        BadBsdStringContextElem);
}

TEST(BsdStringContextElemTest, single_bang_invalid) {
    EXPECT_THROW(
        BsdStringContextElem::parse("!"),
        BadBsdStringContextElem);
}

TEST(BsdStringContextElemTest, double_bang_invalid) {
    EXPECT_THROW(
        BsdStringContextElem::parse("!!/"),
        BadStorePath);
}

TEST(BsdStringContextElemTest, eq_slash_invalid) {
    EXPECT_THROW(
        BsdStringContextElem::parse("=/"),
        BadStorePath);
}

TEST(BsdStringContextElemTest, slash_invalid) {
    EXPECT_THROW(
        BsdStringContextElem::parse("/"),
        BadStorePath);
}

/**
 * Round trip (string <-> data structure) test for
 * `BsdStringContextElem::Opaque`.
 */
TEST(BsdStringContextElemTest, opaque) {
    std::string_view opaque = "g1w7hy3qg1w7hy3qg1w7hy3qg1w7hy3q-x";
    auto elem = BsdStringContextElem::parse(opaque);
    auto * p = std::get_if<BsdStringContextElem::Opaque>(&elem.raw);
    ASSERT_TRUE(p);
    ASSERT_EQ(p->path, StorePath { opaque });
    ASSERT_EQ(elem.to_string(), opaque);
}

/**
 * Round trip (string <-> data structure) test for
 * `BsdStringContextElem::DrvDeep`.
 */
TEST(BsdStringContextElemTest, drvDeep) {
    std::string_view drvDeep = "=g1w7hy3qg1w7hy3qg1w7hy3qg1w7hy3q-x.drv";
    auto elem = BsdStringContextElem::parse(drvDeep);
    auto * p = std::get_if<BsdStringContextElem::DrvDeep>(&elem.raw);
    ASSERT_TRUE(p);
    ASSERT_EQ(p->drvPath, StorePath { drvDeep.substr(1) });
    ASSERT_EQ(elem.to_string(), drvDeep);
}

/**
 * Round trip (string <-> data structure) test for a simpler
 * `BsdStringContextElem::Built`.
 */
TEST(BsdStringContextElemTest, built_opaque) {
    std::string_view built = "!foo!g1w7hy3qg1w7hy3qg1w7hy3qg1w7hy3q-x.drv";
    auto elem = BsdStringContextElem::parse(built);
    auto * p = std::get_if<BsdStringContextElem::Built>(&elem.raw);
    ASSERT_TRUE(p);
    ASSERT_EQ(p->output, "foo");
    ASSERT_EQ(*p->drvPath, ((SingleDerivedPath) SingleDerivedPath::Opaque {
        .path = StorePath { built.substr(5) },
    }));
    ASSERT_EQ(elem.to_string(), built);
}

/**
 * Round trip (string <-> data structure) test for a more complex,
 * inductive `BsdStringContextElem::Built`.
 */
TEST(BsdStringContextElemTest, built_built) {
    /**
     * We set these in tests rather than the regular globals so we don't have
     * to worry about race conditions if the tests run concurrently.
     */
    ExperimentalFeatureSettings mockXpSettings;
    mockXpSettings.set("experimental-features", "dynamic-derivations ca-derivations");

    std::string_view built = "!foo!bar!g1w7hy3qg1w7hy3qg1w7hy3qg1w7hy3q-x.drv";
    auto elem = BsdStringContextElem::parse(built, mockXpSettings);
    auto * p = std::get_if<BsdStringContextElem::Built>(&elem.raw);
    ASSERT_TRUE(p);
    ASSERT_EQ(p->output, "foo");
    auto * drvPath = std::get_if<SingleDerivedPath::Built>(&*p->drvPath);
    ASSERT_TRUE(drvPath);
    ASSERT_EQ(drvPath->output, "bar");
    ASSERT_EQ(*drvPath->drvPath, ((SingleDerivedPath) SingleDerivedPath::Opaque {
        .path = StorePath { built.substr(9) },
    }));
    ASSERT_EQ(elem.to_string(), built);
}

/**
 * Without the right experimental features enabled, we cannot parse a
 * complex inductive string context element.
 */
TEST(BsdStringContextElemTest, built_built_xp) {
    ASSERT_THROW(
        BsdStringContextElem::parse("!foo!bar!g1w7hy3qg1w7hy3qg1w7hy3qg1w7hy3q-x.drv"),        MissingExperimentalFeature);
}

#ifndef COVERAGE

RC_GTEST_PROP(
    BsdStringContextElemTest,
    prop_round_rip,
    (const BsdStringContextElem & o))
{
    ExperimentalFeatureSettings xpSettings;
    xpSettings.set("experimental-features", "dynamic-derivations");
    RC_ASSERT(o == BsdStringContextElem::parse(o.to_string(), xpSettings));
}

#endif

}
