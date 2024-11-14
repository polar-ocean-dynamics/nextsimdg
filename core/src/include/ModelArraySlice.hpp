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
    ModelArraySlice& operator=(const double* v);

    Slice slice;
private:
    static void copySliceWithIters(ModelArray& source, Slice::SliceIter& sourceIter, ModelArray& target, Slice::SliceIter targetIter);
    ModelArray& data;
};

} // namespace Nextsim

#endif /* MODELARRAYSLICE_HPP */
