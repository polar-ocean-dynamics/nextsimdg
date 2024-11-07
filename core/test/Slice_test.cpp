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
    Slice element3 {{{3U}}};
    Slice::SliceIter iter3(element3, {10});
    REQUIRE(iter3.index() == 3);

    // A range
    Slice range36 {{{3, 6}}};
    Slice::SliceIter iter36(range36, {10});
    REQUIRE(iter36.index() == 3);
    REQUIRE(iter36.isBegin());
    ++iter36;
    REQUIRE(iter36.index() == 4);
    // post incrementing
    REQUIRE(iter36++.index() == 4);
    REQUIRE(iter36.index() == 5);
    ++iter36;
    REQUIRE(iter36.isEnd());
    // Increment past the end
    ++iter36;

    // range to the end of the array
    Slice range3_ {{{3, {}}}};
    Slice::SliceIter iter3_(range3_, {10});
    // count the values until the end 3456789
    size_t count = 0;
    while(!iter3_.isEnd()) {
        ++iter3_;
        ++count;
        REQUIRE(count < 10);
    }
    REQUIRE(iter3_.isEnd());
    REQUIRE(count == 7);


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

    // A multidimensional slice
    Slice elements3748 {{{3, 7}, {4, 8}}};
    const size_t nx = 11;
    const size_t ny = 13;
    Slice::SliceIter iter3748(elements3748, {nx, ny});
    size_t index = iter3748.index();
    REQUIRE(index == Indexer::indexer({nx, ny}, {3, 4}));
    size_t count = 0;
    size_t indexLast = index - 1;
    while(!iter3748.isEnd()) {
        index = iter3748.index();
        std::ptrdiff_t deltaIndex = index - indexLast;
        bool xStep = deltaIndex == 1;
        bool yStep = deltaIndex == (Indexer::indexer({nx, ny}, {3, 5}) - Indexer::indexer({nx, ny}, {6, 4}));
        bool allowedStep = xStep || yStep;
        REQUIRE_MESSAGE(allowedStep, "Forbidden step value Δi=", deltaIndex);
        REQUIRE(index % nx >= 3);
        REQUIRE(index % nx < 7);
        REQUIRE(index / nx >= 4);
        REQUIRE(index / nx < 8);
        REQUIRE(count <= 16);
        indexLast = index;
        ++iter3748;
        ++count;
    }
}

TEST_SUITE_END();
