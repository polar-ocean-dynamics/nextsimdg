/*!
 * @file ModelArraySlice_test.cpp
 *
 * @date Nov 8, 2024
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "include/ModelArraySlice.hpp"

const auto nx = 23;
const auto ny = 17;

namespace Nextsim {
TEST_SUITE_BEGIN("ModelArraySlice");
TEST_CASE("Assign to slice")
{
    ModelArray::setDimension(ModelArray::Dimension::X, nx);
    ModelArray::setDimension(ModelArray::Dimension::Y, ny);

    TwoDField target(ModelArray::Type::TWOD);
    target.resize();
    double orig = 0.;
    target = orig;
    Slice patch {{{5, 14}, {8, 15}}};
    ModelArraySlice mas(target, patch);
    double set = 1.;
    mas = set;
    REQUIRE(target(5, 8) == set);
    REQUIRE(target(13, 8) == set);
    REQUIRE(target(5, 14) == set);
    REQUIRE(target(13, 14) == set);

    REQUIRE(target(4, 8) == orig);
    REQUIRE(target(5, 7) == orig);
    REQUIRE(target(14, 8) == orig);
    REQUIRE(target(13, 7) == orig);
    REQUIRE(target(4, 14) == orig);
    REQUIRE(target(5, 15) == orig);
    REQUIRE(target(14, 14) == orig);
    REQUIRE(target(13, 15) == orig);
}

TEST_CASE("Slice to Slice")
{
    ModelArray::setDimension(ModelArray::Dimension::X, nx);
    ModelArray::setDimension(ModelArray::Dimension::Y, ny);

    TwoDField source(ModelArray::Type::TWOD);
    TwoDField target(ModelArray::Type::TWOD);
}
TEST_SUITE_END();
} // namespace Nextsim
