#include <algorithm>
#include <cstddef>
#include <limits>
#include <ostream>
#include <vector>

#include "include/indexer.hpp"

// A macro definition of the two-argument ceiling function
#define ceil(num, denom) (((num) + (denom) - (((denom) < 0) ? -1 : 1)) / (denom))
namespace ArraySlicer {
class SliceIter;

/*!
 * @brief A class to hold the bounds of a general slice, not tied to any
 * particular array.
 *
 * @details The Slice class allows the definition of a slice of an array. The
 * format used mirrors that of the Python numpy library as far as possible. The
 * slicing admits single element indexing with the syntax {{{i}}}. This defines
 * a slice that accesses only a single element of a one dimensional array. The
 * index can also be absent, as {{{}}}, which is used to designate the entire
 * one-dimensional array. This can also be explicitly defined using an empty
 * initializer list, as {{{{}}}}.
 *
 * Beyond indexing a single element the slice syntax can specify a range. This
 * consists of a start index, an optional stop index and an optional step
 * index. If both stop and step are absent, then the slice reverts to single
 * element addressing. The stop index is the index of the element one past the
 * end of the desired slice. The number of elements in the slice is given by
 * stop - start if the step is 1 (its default value). This means that a slice
 * with a start of 2 and a stop of 7 will have five elements at indices
 * 2, 3, 4, 5 & 6. The syntax for a slice with both a start and a stop is
 * {{{start, stop}}}. Either can also be replaced by an empty initializer list,
 *  as {{{{}, stop}}}, {{{start, {}}}} and {{{{}, {}}}}. A default start runs
 *  from the start of the array and a default stop runs to the end of the
 *  array. If both are default empty initializer lists, then the slice spans
 *  the entire array.
 *
 *  A step value can also be present. When traversing the slice, the index of
 *  the location in the array will increase by the step value. Accessing the
 *  start=2 and stop=7 slice described above with a step of 2 would be written
 *  as {{{2, 7, 2}}} and access the elements at indices 2, 4 & 6. The syntax
 *  for a slice with a start, stop and step is {{{start, stop, step}}}. Both
 *  start and stop must be present to define a step using the default
 *  initializer list syntax to give {{{{}, {}, step}}}. The step value can be
 *  negative. This requires the start value to be greater than the stop value
 *  to avoid an empty slice. The slice written as {{{2, 7, -2}} defines an
 *  empty slice. The slice written as {{{7, 2, -2}}} would access the indices
 *  7, 5 & 3.
 *
 *  The start and stop indices can also be negative, which are interpreted as
 *  counting backwards from the end of the array. The element -1 is the last
 *  element in the array, -2 is the second last all the way to -n, the first
 *  element of an array of length n.
 *
 *  The above description details how indexing works on a one dimensional
 *  array. A slice can also be defined over a multidimensional array of
 *  arbitrarily high dimension. This is done by adding additional bounds
 *  initializers to the second set of braces. If the suffix _n is applied to
 *  the start, stop and step indices in dimension n, then a fully specified
 *  multidimensional slice is written as
 *  {{{start_0, stop_0, step_0}, {start_1, stop_1, step_1}…}}. The bounds term
 *  for each dimensional can be specified in the same way as for the one
 *  dimensional bounds described above, including default initializer list
 *  values, negative indices and negative step values.
 *
 */
class Slice {
public:
    using Int = std::ptrdiff_t;
    //  Bounds for one dimension
    class Bounds {
    public:
        static const size_t max = std::numeric_limits<size_t>::max();

    private:
        class Index {
        public:
            Index()
                : m_isAll(true)
                , i(0)
            {
            }
            Index(Int ii)
                : m_isAll(false)
                , i(ii)
            {
            }
            bool isAll() const { return m_isAll; }
            operator std::ptrdiff_t() const { return i; }

            std::ostream& print(std::ostream& os) const
            {
                return os << (m_isAll ? "all" : std::to_string(i));
            }

        private:
            bool m_isAll;
            Int i;
        };

    public:
        Index start;
        Index stop;
        Int step;
        Bounds()
            : Bounds(0, {}, 1)
        {
        }
        Bounds(Index i)
            : Bounds((i.isAll()) ? Bounds() : Bounds(i, i + 1, 1))
        {
        }
        Bounds(Index i, Index j)
            : Bounds(i, j, 1)
        {
        }
        Bounds(Index i, Index j, Int step_in)
            : start(i)
            , stop(j)
            , step(step_in)
        {
            if (!step)
                throw std::invalid_argument(
                    "Slice::Bounds::Bounds(Index, Index, Int): slice step cannot be zero");
        }
        std::ostream& print(std::ostream& os) const
        {
            return os << start << ":" << stop << ":" << step;
        }
        friend SliceIter;
    };

public:
    using VBounds = std::vector<Bounds>;
    const VBounds bounds;

public:
    Slice()
        : Slice({ Bounds() })
    {
    }
    Slice(VBounds allBounds)
        : bounds(allBounds)
    {
    }

    const size_t n() const { return bounds.size(); }
};

inline std::ostream& operator<<(std::ostream& os, const Slice::Bounds& bounds)
{
    return bounds.print(os);
}

inline std::ostream& operator<<(std::ostream& os, const Slice::VBounds& vBounds)
{
    os << "{";
    for (size_t i = 0; i < vBounds.size(); ++i) {
        os << vBounds[i] << ((i != vBounds.size() - 1) ? "," : "");
    }
    return os << "}";
}

inline std::ostream& operator<<(std::ostream& os, const Slice& slice) { return os << slice.bounds; }

class SliceIter {
public:
    using MultiDim = std::vector<size_t>;
    using Int = Slice::Int;
    SliceIter(const Slice& slice, const MultiDim& dimensions)
        : m_slice(realiseIndices(slice.bounds, dimensions))
        , m_dimensions(dimensions)
        , current(dimensions.size(), 0)
    {
        toBegin();
        if (slice.n() != dimensions.size())
            throw std::invalid_argument(
                "SliceIter: mismatch in number of dimensions between Slice ("
                + std::to_string(slice.n()) + ") and extent (" + std::to_string(dimensions.size())
                + ").");
    }

    bool operator==(const SliceIter& other) const
    {
        // Number of dimensions must match
        if (m_slice.n() != other.m_slice.n())
            return false;
        const size_t ndim = m_slice.n();
        // if only one isEnd, then the iterators cannot be equal
        if (isEnd() != other.isEnd())
            return false;
        for (size_t dim = 0; dim < ndim; ++dim) {
            // Dimension length must match
            if (m_dimensions[dim] != other.m_dimensions[dim])
                return false;
            // Dimension limits must match
            if (m_slice.bounds[dim].start != other.m_slice.bounds[dim].start)
                return false;
            if (m_slice.bounds[dim].stop != other.m_slice.bounds[dim].stop)
                return false;
            if (m_slice.bounds[dim].step != other.m_slice.bounds[dim].step)
                return false;
            // Position must match, if we are not at the end
            if (!isEnd()) {
                if (current[dim] != other.current[dim])
                    return false;
            }
        }
        return true;
    }

    SliceIter& operator++() { return incrementDim(0); }
    SliceIter operator++(int)
    {
        SliceIter copy(*this);
        ++(*this);
        return copy;
    }

    SliceIter& incrementDim(size_t dim)
    {
        for (;;) {
            // Increment current in this dimension, and test if it is at or past the end.
            if (!stopTest(current[dim] += m_slice.bounds[dim].step, dim))
                break;
            // Never reset the final valid dimension.
            if (dim + 1 >= current.size())
                break;
            // Reset the dimension to the start
            current[dim] = m_slice.bounds[dim].start;
            // Increment dimension
            ++dim;
        }
        return *this;
    }

    SliceIter& operator--();
    SliceIter operator--(int);
    /*!
     * Returns the one-dimensional index equivalent to the current state of the iterator
     */
    Int index() const { return Indexer::indexer(m_dimensions, current); }
    /*!
     * Sets the state to the beginning of the slice
     */
    SliceIter& toBegin()
    {
        for (size_t dim = 0; dim < m_slice.n(); ++dim) {
            current[dim] = m_slice.bounds[dim].start;
        }
        return *this;
    }
    /*!
     * Sets the state to the end of the slice.
     */
    SliceIter& toEnd()
    {
        size_t lastDim = m_slice.n() - 1;
        for (size_t dim = 0; dim < lastDim; ++dim) {
            current[dim] = 0;
        }
        current[lastDim] = dimEnd(lastDim);
        return *this;
    }
    /*!
     * Is the iterator at its 'begin'?
     */
    bool isBegin() const
    {
        for (size_t dim = 0; dim < m_slice.n(); ++dim) {
            if (current[dim] != m_slice.bounds[dim].start)
                return false;
        }
        return true;
    }
    /*!
     * Is the iterator at its 'end'?
     */
    bool isEnd() const
    {
        size_t lastDim = m_slice.n() - 1;
        return stopTest(current, lastDim);
    }

    bool& doDetailedOutput()
    {
        static bool ddo = false;
        return ddo;
    }

    /*!
     * Returns the overall shape of the Slice, constrained by the given dimensions.
     */
    MultiDim shape()
    {
        MultiDim shapey;

        // TODO handle negative indices
        shapey.resize(m_slice.bounds.size());
        for (size_t dim = 0; dim < m_slice.bounds.size(); ++dim) {
            shapey[dim] = nElements(dim);
        }

        return shapey;
    }

    /*!
     * Translates the default and negative bounds into an actual start index.
     * @param dim the dimension for which the start value is requested.
     */
    Int start(size_t dim) const
    {
        // TODO handle negative indices
        return (m_slice.bounds[dim].start.isAll()) ? ((step(dim) < 0) ? m_dimensions[dim] - 1 : 0)
                                                   : static_cast<size_t>(m_slice.bounds[dim].start);
    }
    /*!
     * Translates the default and negative bounds into an actual start
     * index for the first dimension.
     */
    Int start() const
    {
        if (m_slice.n() != 1) {
            throw std::out_of_range(
                "Slice::start(): use Slice::start(size_t) for multi-dimensional slices.");
        }
        return start(0);
    }

    /*!
     * Calculates the number of elements in the the slice along this dimension.
     * @param dim the dimension for which the number of elements is requested.
     */
    Int nElements(size_t dim) const
    {
        // TODO handle negative indices
        Int stop = (m_slice.bounds[dim].stop.isAll())
            ? ((step(dim) < 0) ? -1 : m_dimensions[dim])
            : static_cast<size_t>(m_slice.bounds[dim].stop);
        return ceil(stop - start(dim), step(dim));
    }
    /*!
     * Calculates the number of elements in the the slice along the first dimension.
     */
    Int nElements() const
    {
        if (m_slice.n() != 1) {
            throw std::out_of_range(
                "Slice::nElements(): use Slice::nElements(size_t) for multi-dimensional slices.");
        }
        return nElements(0);
    }

    Int step(size_t dim) const { return m_slice.bounds[dim].step; }
    Int step() const
    {
        if (m_slice.n() != 1) {
            throw std::out_of_range(
                "Slice::step(): use Slice::step(size_t) for multi-dimensional slices.");
        }
        return step(0);
    }

    std::ostream& print(std::ostream& os) const
    {
        size_t ndims = m_slice.n();
        size_t noComma = ndims - 1;
        // Print the bounds
        os << m_slice << "[";
        for (size_t i = 0; i < ndims; ++i) {
            os << current[i] << ((i < noComma) ? "," : "");
        }
        os << "]";
        return os;
    }

private:
    Int dimEnd(size_t dim) const
    {
        return (m_slice.bounds[dim].stop.isAll())
            ? ((step(dim) < 0) ? -1 : m_dimensions[dim])
            : static_cast<Slice::Int>(m_slice.bounds[dim].stop);
    }
    // Test whether a dimension has run past the end of its bounds
    bool stopTest(Int subject, size_t dim) const
    {
        return (step(dim) < 0) ? subject <= dimEnd(dim) : subject >= dimEnd(dim);
    }
    bool stopTest(const MultiDim& loc, size_t dim) const { return stopTest(loc[dim], dim); }

    static Slice::VBounds realiseIndices(const Slice::VBounds& in, const MultiDim& dims)
    {

        Slice::VBounds out;
        out.resize(in.size());
        // Iterate over the bounds, replacing isAll with the actual bounds
        for (size_t i = 0; i < in.size(); ++i) {
            const Int signedDim = static_cast<Int>(dims[i]);
            const Slice::Bounds::Index indexDim = static_cast<Slice::Bounds::Index>(signedDim);
            const Int int0 = static_cast<Int>(0);
            // start
            if (in[i].start.isAll()) {
                out[i].start = (in[i].step < 0) ? dims[i] - 1 : 0;
            } else if (in[i].start < 0) {
                out[i].start = (in[i].step < 0) ? std::min(signedDim + in[i].start, signedDim)
                                                : std::max(signedDim + in[i].start, int0);
            } else {
                out[i].start = in[i].start;
            }
            // stop
            if (in[i].stop < 0) {
                if ((signedDim + in[i].stop < 0) && in[i].step < 0) {
                    // Set isAll() = true
                    out[i].stop = Slice::Bounds::Index();
                } else {
                    out[i].stop = std::max(signedDim + in[i].stop, int0);
                }
            } else {
                out[i].stop = (in[i].stop > signedDim) ? indexDim : in[i].stop;
            }
            // step
            out[i].step = in[i].step;
        }
        return out;
    }

    const MultiDim m_dimensions;
    MultiDim current;
    const Slice m_slice;
};

inline std::ostream& operator<<(std::ostream& os, const SliceIter& si) { return si.print(os); }
} // namespace ArraySlicer

#undef ceil
