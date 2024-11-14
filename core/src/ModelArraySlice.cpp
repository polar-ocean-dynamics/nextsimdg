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

    copySliceWithIters(other.data, otherIter, data, thisIter);
    return *this;
}

void ModelArraySlice::copySliceWithIters(ModelArray& source, Slice::SliceIter& sourceIter, ModelArray& target, Slice::SliceIter targetIter)
{
    const size_t targetNEl = targetIter.nElements(0);
    const size_t sourceNEl = sourceIter.nElements(0);

    while(!targetIter.isEnd() && !sourceIter.isEnd())
    {
        const size_t targetIndex = targetIter.index();
        const size_t sourceIndex = sourceIter.index();
        target.m_data(Eigen::seqN(targetIndex, targetNEl, targetIter.step(0)), Eigen::all) =
                source.m_data(Eigen::seqN(sourceIndex, sourceNEl, sourceIter.step(0)), Eigen::all);
        targetIter.incrementDim(1);
        sourceIter.incrementDim(1);
    }
}
} // namespace Nextsim
