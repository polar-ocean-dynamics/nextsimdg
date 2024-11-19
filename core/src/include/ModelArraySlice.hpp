/*!
 * @file ModelArraySlice.hpp
 *
 * @date Nov 8, 2024
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#ifndef MODELARRAYSLICE_HPP
#define MODELARRAYSLICE_HPP

#include "include/ModelArray.hpp"
#include "include/Slice.hpp"

namespace Nextsim {

class ModelArraySlice {
public:
    using Slice = ArraySlicer::Slice;
    using SliceIter = ArraySlicer::SliceIter;
    ModelArraySlice() = delete;
    ModelArraySlice(ModelArray& ma, const Slice& sl)
        : data(ma)
        , slice(sl)
    {
    }

    // Assign data to this slice from a ModelArray
    ModelArraySlice& operator=(ModelArray& ma);
    // Copy data from another ModelArraySlice
    ModelArraySlice& operator=(ModelArraySlice& other);
    // Assign a scalar to the entire slice
    ModelArraySlice& operator=(double v);
    // Assign the contents of a buffer to a slice
    template <typename T> ModelArraySlice& operator=(const T& buffer)
    {
        // make no especial attempt at efficiency here
        SliceIter thisIter(slice, data.dimensions());
        auto biter = buffer.begin();

        while (!thisIter.isEnd()) {
            // If the buffer ends before the slice, throw an exception
            if (biter == buffer.end()) {
                throw std::length_error("ModelArraySlice::operator=(T): buffer exhausted");
            }
            data[thisIter.index()] = *biter;
            ++thisIter;
            ++biter;
        }
        return *this;
    }

    ModelArray& copyToModelArray(ModelArray& target) const;
    template <typename T> T& copyToBuffer(T& buffer)
    {
        // make no especial attempt at efficiency here
        SliceIter thisIter(slice, data.dimensions());
        auto biter = buffer.begin();

        while (!thisIter.isEnd()) {
            // If the buffer ends before the slice, throw an exception
            if (biter == buffer.end()) {
                throw std::length_error("ModelArraySlice::copyToBuffer(T): buffer exhausted");
            }
            *biter = data[thisIter.index()];
            ++thisIter;
            ++biter;
        }
        return buffer;
    }

    Slice slice;

private:
    static void copyBetweenMAandMASlice(
        ModelArray& ma, const ModelArraySlice& mas, bool toSlice, const std::string& functionName);
    static void copySliceWithIters(
        ModelArray& source, SliceIter& sourceIter, ModelArray& target, SliceIter targetIter);
    ModelArray& data;
};

} // namespace Nextsim

#endif /* MODELARRAYSLICE_HPP */
