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

#define ciel(num , denom) (((num) + (denom) - 1) / (denom))

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

    // Non-unit stride, also full length of the array.
    Slice stride2 {{{{},{},2}}};
    Slice::SliceIter iter__2(stride2, {10});
    REQUIRE(iter__2.index() == 0);
    count = 0;
    size_t expt = 10 / 2;
    while (!iter__2.isEnd()) {
        REQUIRE(iter__2.index() % 2 == 0);
        ++iter__2;
        ++count;
    }
    REQUIRE(count == expt);

    // And starting from 1
    Slice stride2a = {{{1, {}, 2}}};
    Slice::SliceIter iter1_2(stride2a, {10});
    REQUIRE(iter1_2.index() == 1);
    count = 0;
    // expt still = 5
    while (!iter1_2.isEnd()) {
        REQUIRE(iter1_2.index() % 2 == 1);
        ++iter1_2;
        ++count;
    }
    REQUIRE(count == expt);

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

    // 8 dimensional array slicing
    Slice elements8d {{{2, 6}, {4, 9}, {6, 12}, {8, 15}, {10, 18}, {12, 21}, {14, 24}, {16, 27}}};
    std::vector<size_t> ni = { 7, 12, 13, 16, 30, 30, 30, 30};
    Slice::SliceIter iter8d(elements8d, ni);
    count = 0;
    const size_t expt = 11 * 10 * 9 * 8 * 7 * 6 * 5 * 4;
    while (!iter8d.isEnd()) {
        auto loc = Indexer::deIndexer(ni, iter8d.index());
        for (size_t dim = 0; dim < ni.size(); ++dim) {
            REQUIRE(loc[dim] >= elements8d.bounds[dim].start);
            REQUIRE(loc[dim] < elements8d.bounds[dim].stop);
        }
        ++iter8d;
        ++count;
    }
    REQUIRE(count == expt);

    // n to whatever in 2 dimensions
    const size_t xi = 3;
    const size_t yi = 5;
    Slice slice3_5_ {{{xi, {}}, {yi, {}}}};
    std::vector<std::vector<size_t>> dims = { { 11, 17 }, { 37, 43 } };
    for (std::vector<size_t> nj : dims) {
        count = 0;
        for (Slice::SliceIter iter(slice3_5_, nj); !iter.isEnd(); ++iter) {
            ++count;
        }
        REQUIRE(count == (nj[0] - xi) * (nj[1] - yi));
    }

    // Multiple dimensions, multiple strides
    auto dx = 2;
    auto dy = 3;
    auto lenY = 13;
    Slice sliceMultiStride {{{xi, {}, dx}, {yi, yi + lenY, dy}}};
    count = 0;
    auto dim = dims[1]; // Take the arrays sizes from above
    for (Slice::SliceIter iter(sliceMultiStride, dim); !iter.isEnd(); ++iter) {
        count++;
    }
    REQUIRE(count == ciel(dim[0] - xi, dx) * ciel(lenY, dy));

    // Reuse elements8d to test higher dimensional incrementing
    size_t i1, i2;
    iter8d.toBegin();
    i1 = iter8d.index();
    i2 = (++iter8d).index();
    REQUIRE(Indexer::deIndexer(ni, i2)[0] == Indexer::deIndexer(ni, i1)[0] + 1);
    i1 = iter8d.incrementDim(1).index();
    REQUIRE(Indexer::deIndexer(ni, i1)[1] == Indexer::deIndexer(ni, i2)[1] + 1);
    i2 = iter8d.incrementDim(7).index();
    REQUIRE(Indexer::deIndexer(ni, i2)[7] == Indexer::deIndexer(ni, i1)[7] + 1);
}

TEST_SUITE_END();
