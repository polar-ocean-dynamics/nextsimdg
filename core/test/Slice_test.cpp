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
    Slice element3 {{{3}}};
    Slice::SliceIter iter3(element3, {10});
    REQUIRE(iter3.index() == 3);
    Slice allOneD;
}

TEST_CASE("Multidimensional indexing")
{
    // A single element
    Slice element {{{3}, {5}}};
    Slice::SliceIter iter610(element, {6, 10});
    REQUIRE(iter610.index() == Indexer::indexer({6, 10}, {3, 5}));
    REQUIRE(Slice::SliceIter(element, {8, 7}).index() == Indexer::indexer({8, 7}, {3, 5}));

    // Check that a mismatch in number of dimensions is correctly detected
    REQUIRE_THROWS_AS(Slice::SliceIter(element, {8, 7, 6}), std::invalid_argument);
}

TEST_SUITE_END();
