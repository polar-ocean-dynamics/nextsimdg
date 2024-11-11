/*!
 * @file ModelArraySlice.cpp
 *
 * @date Nov 11, 2024
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#include "include/ModelArraySlice.hpp"

namespace Nextsim {

ModelArraySlice& ModelArraySlice::operator=(double v)
{
    /*
     * The left most index varies fastest, as is the ModelArray standard
     * outside of components. This can be directly used as arguments to an
     * Eigen::seq object
     */
    Slice::SliceIter si(slice, data.dimensions());
    while (!si.isEnd()) {
        const size_t index = si.index();
        data.m_data(Eigen::seq(index, index - 1 + slice.bounds[0].stop - slice.bounds[0].start, slice.bounds[0].step), Eigen::all) = v;
        si.incrementDim(1);
    }
    return *this;
}

ModelArraySlice& ModelArraySlice::operator=(ModelArraySlice& other)
{
    Slice::SliceIter thisIter(slice, data.dimensions());
    Slice::SliceIter otherIter(other.slice, other.data.dimensions());
    // Check that the shapes match
    auto thisShape = thisIter.shape();
    auto otherShape = otherIter.shape();
    if (thisShape.size() != otherShape.size())
        throw std::out_of_range("ModelArraySlice dimension mismatch");
    for (auto i = 0; i < thisShape.size(); ++i) {
        if (thisShape[i] != otherShape[i])
            throw std::out_of_range("ModelArraySlice shape mismatch");
    }
        return *this;
}

} // namespace Nextsim
