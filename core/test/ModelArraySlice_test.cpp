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
TEST_CASE("Assign scalar to slice")
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
    source.resize();
    TwoDField target(ModelArray::Type::TWOD);
    target.resize();
    double targetv = -1.;
    target = targetv;

    for (size_t i = 0; i < source.size(); ++i) {
        source[i] = i;
    }

    // First test the error conditions
    ModelArraySlice masa(source, {{{1, 3}, {{}, 19}}});
    ModelArraySlice masb(source, {{{1, 3}, {8, 19}}});
    REQUIRE_THROWS_AS(masa = masb, std::out_of_range);

    size_t x0s = 0;
    size_t x1s = 3;
    size_t y0s = 0;
    size_t y1s = 14;

    size_t x0t = nx - x1s;
    size_t x1t = nx;
    size_t y0t = ny - y1s;
    size_t y1t = ny;
    ModelArraySlice sourceSlice(source, {{{x0s, x1s}, {y0s, y1s}}});
    ModelArraySlice targetSlice(target, {{{x0t, x1t}, {y0t, y1t}}});

    // Perform the Slice to Slice assignment
    targetSlice = sourceSlice;

    // Test the areas that should not be assigned to
    REQUIRE(target(x0t-1, y0t) == targetv);
    REQUIRE(target(x0t, y0t-1) == targetv);
    REQUIRE(target(x1t - 1, y0t - 1) == targetv);
    REQUIRE(target(x0t - 1, y0t - 1) == targetv);

    // Test the areas that should be assigned to
    REQUIRE(target(x0t, y0t) != targetv);
    REQUIRE(target(x0t, y0t) == source(x0s, y0s));
    REQUIRE(target(x0t, y1t - 1) == source(x0s, y1s - 1));
    REQUIRE(target(x1t - 1, y0t) == source(x1s - 1, y0s));
    REQUIRE(target(x1t - 1, y1t - 1) == source(x1s - 1, y1s - 1));

    /*****************************************************************/
    // Test negative step
    target = targetv;

    x0s = 0;
    x1s = 3;
    y0s = 3;
    y1s = ny-3;

    x0t = nx - 1;
    x1t = x0t - (x1s - x0s);
    Slice::Int xstep = -1;
    y0t = y1s - 1;
    y1t = y0s - 1;
    Slice::Int ystep = -1;

    ModelArraySlice sourceSlice2(source, {{{x0s, x1s}, {y0s, y1s}}});
    ModelArraySlice targetSlice2(target, {{{x0t, x1t, -1}, {y0t, y1t, -1}}});

    targetSlice2 = sourceSlice2;

    // Test the areas that should not be assigned to
    REQUIRE(target(x1t, y1t+1) == targetv);
    REQUIRE(target(x1t+1, y1t) == targetv);
    REQUIRE(target(x0t, y1t) == targetv);
    REQUIRE(target(x0t, y0t+1) == targetv);

    // Test the areas that should be assigned to
    REQUIRE(target(x0t, y0t) != targetv);
    REQUIRE(target(x0t, y0t) == source(x0s, y0s));
    REQUIRE(target(x0t, y1t - ystep) == source(x0s, y1s - 1));
    REQUIRE(target(x1t - xstep, y0t) == source(x1s - 1, y0s));
    REQUIRE(target(x1t - xstep, y1t - ystep) == source(x1s - 1, y1s - 1));

}

TEST_SUITE_END();
} // namespace Nextsim
