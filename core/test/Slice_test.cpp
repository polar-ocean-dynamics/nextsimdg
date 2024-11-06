/*!
 * @file Slice_test.cpp
 *
 * @date 5 Nov 2024
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "include/Slice.hpp"


TEST_SUITE_BEGIN("Slice");
TEST_CASE("One dimensional indexing")
{
    // A single element
    Slice element3 {{3, 4}};
    Slice::SliceIter iter3(element3, {10});
    REQUIRE(iter3.toBegin().index() == 3);
    Slice allOneD;
}

TEST_SUITE_END();
