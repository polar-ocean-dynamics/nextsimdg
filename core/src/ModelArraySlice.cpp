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
        data.m_data(Eigen::seqN(index, si.nElements(0), si.step(0)), Eigen::all) = v;
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

    const size_t thisNEl = thisIter.nElements(0);
    const size_t otherNEl = otherIter.nElements(0);

    while(!thisIter.isEnd() && !otherIter.isEnd())
    {
        const size_t thisIndex = thisIter.index();
        const size_t otherIndex = otherIter.index();
        data.m_data(Eigen::seqN(thisIndex, thisNEl, thisIter.step(0)), Eigen::all) =
                other.data.m_data(Eigen::seqN(otherIndex, otherNEl, otherIter.step(0)), Eigen::all);
        thisIter.incrementDim(1);
        otherIter.incrementDim(1);
    }
    return *this;
}

} // namespace Nextsim
