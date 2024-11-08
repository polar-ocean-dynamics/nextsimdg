/*!
 * @file Slice_test.cpp
 *
 * @date 5 Nov 2024
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "include/Slice.hpp"

#include <array>

TEST_SUITE_BEGIN("Indexer");
TEST_CASE("indexer <-> deIndexer")
{
    using MultiDim = std::vector<size_t>;
    MultiDim origLoc = { 2, 3, 5, 7, 11, 13 };
    MultiDim dims = { 4, 6, 8, 10, 12, 14 };

    size_t index = Indexer::indexer(dims, origLoc);
    MultiDim finalLoc = Indexer::deIndexer(dims, index);
    for (size_t i = 0; i < dims.size(); ++i) {
        REQUIRE(origLoc[i] == finalLoc[i]);
    }
}
TEST_SUITE_END();

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

    Slice elements8d {{{2, 6}, {4, 9}, {6, 12}, {8, 15}, {10, 18}, {12, 21}, {14, 24}, {16, 27}}};
    std::vector<size_t> ni = { 7, 12, 13, 16, 30, 30, 30, 30};
    Slice::SliceIter iter8d(elements8d, ni);
    count = 0;
    const size_t expt = 11 * 10 * 9 * 8 * 7 * 6 * 5 * 4;
    while (!iter8d.isEnd()) {
        auto loc = Indexer::deIndexer(ni, iter8d.index());
        for (size_t dim = 0; dim < ni.size(); ++dim) {
            REQUIRE(loc[dim] >= elements8d.bounds()[dim].start);
            REQUIRE(loc[dim] < elements8d.bounds()[dim].stop);
        }
        ++iter8d;
        ++count;
    }
    REQUIRE(count == expt);
}

TEST_SUITE_END();
