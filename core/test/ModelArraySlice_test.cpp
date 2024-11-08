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
    target = 0;
    Slice patch {{{5, 14}, {8, 15}}};
    ModelArraySlice mas(target, patch);
    mas = 1.;
    REQUIRE(target(5, 8) == 1.);
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
