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

#include <iostream>
#include <iterator>

namespace Nextsim {
class MASIter;
class ModelArraySlice;
std::ostream& operator<<(std::ostream& os, const MASIter& it);

// Inheriting from std::iterator is deprecated after C++17
class MASIter {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = double;
    using difference_type = std::ptrdiff_t;
    using pointer = double*;
    using reference = double&;
    using Slice = ArraySlicer::Slice;
    using SliceIter = ArraySlicer::SliceIter;

    MASIter(ModelArraySlice& mas);
    double& operator*() const { return data[iter.index()]; }
    MASIter& operator++()
    {
        ++iter;
        return *this;
    }
    bool operator!=(const MASIter& other) const
    {
        //        std::cout << this->iter << "!=" << other.iter << "?" << std::endl;
        // Account for different end iters possibly not equating
        bool iterEquality = (iter == other.iter) || (iter.isEnd() && other.iter.isEnd());
        return (&data != &other.data) || !iterEquality;
    }
    bool operator==(const MASIter& other) const { return !(*this != other); }
    double* operator->() const { return &data[iter.index()]; }
    MASIter operator++(int)
    {
        MASIter copy = *this;
        ++(*this);
        return copy;
    }

    std::ostream& print(std::ostream& os) const { return os << "{" << &data << ":" << iter << "}"; }
    friend ModelArraySlice;

private:
    ModelArray& data;
    SliceIter iter;
};

inline std::ostream& operator<<(std::ostream& os, const MASIter& it) { return it.print(os); }
class ModelArraySlice {
public:
    using Slice = ArraySlicer::Slice;
    using SliceIter = ArraySlicer::SliceIter;
    using iterator = MASIter;
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

private:
    template <typename S, typename T=S>
    static void copySliceWithIters(const S& source, SliceIter& sourceIter,
        T& target, SliceIter targetIter)
    {
        const size_t targetNEl = targetIter.nElements(0);
        const size_t sourceNEl = sourceIter.nElements(0);

        while (!targetIter.isEnd() && !sourceIter.isEnd()) {
            const size_t targetIndex = targetIter.index();
            const size_t sourceIndex = sourceIter.index();
            target(Eigen::seqN(targetIndex, targetNEl, targetIter.step(0)), Eigen::all)
                = source(Eigen::seqN(sourceIndex, sourceNEl, sourceIter.step(0)), Eigen::all);
            targetIter.incrementDim(1);
            sourceIter.incrementDim(1);
        }
    }

public:
    template <typename T>
    const ModelArraySlice& copyToSlicedBuffer(T& target, SliceIter& targetIter) const
    {
        SliceIter iter(slice, data.dimensions());

        copySliceWithIters(data.m_data, iter, target, targetIter);
        // Return this, even though it is unchanged. The code looks weird if
        // the return value is the target buffer.
        return *this;
    }

    template <typename S>
    ModelArraySlice& copyFromSlicedBuffer(const S& source, SliceIter& sourceIter)
    {
        SliceIter iter(slice, data.dimensions());

        copySliceWithIters(source, sourceIter, data.m_data, iter);
        return *this;
    }

    iterator begin();
    iterator end();

    Slice slice;

    friend MASIter;

private:
    static void copyBetweenMAandMASlice(
        ModelArray& ma, const ModelArraySlice& mas, bool toSlice, const std::string& functionName);

    ModelArray& data;
};

} // namespace Nextsim

#endif /* MODELARRAYSLICE_HPP */
