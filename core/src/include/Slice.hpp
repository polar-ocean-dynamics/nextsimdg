#include <cstddef>
#include <limits>
#include <vector>

#include "include/indexer.hpp"

// Generic n-dimensional slice
class Slice {
    // public:
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
            Index(size_t ii)
                : m_isAll(false)
                , i(ii)
            {
            }
            bool isAll() const { return m_isAll; }
            operator size_t() const { return i; }

        private:
            bool m_isAll;
            size_t i;
        };

    public:
        using NegIndex = std::ptrdiff_t;
        size_t start;
        size_t stop;
        ptrdiff_t step;
        bool reverseStart;
        bool reverseStop;
        Bounds()
            : Bounds(0, max, 1)
        {
        }
        Bounds(Index i)
            : Bounds(i, max, 1)
        {
        }
        Bounds(NegIndex n)
            : Bounds(n, max, 1)
        {
        }
        Bounds(Index i, Index j)
            : Bounds(i, j, 1)
        {
        }
        Bounds(NegIndex n, Index j)
            : Bounds(n, j, 1)
        {
        }
        Bounds(Index i, NegIndex m)
            : Bounds(i, m, 1)
        {
        }
        Bounds(NegIndex n, NegIndex m)
            : Bounds(n, m, 1)
        {
        }
        Bounds(Index i, Index j, NegIndex l)
            : start(i.isAll() ? 0 : static_cast<size_t>(i))
            , stop(j.isAll() ? max : static_cast<size_t>(j))
            , step(l)
            , reverseStart(false)
            , reverseStop(false)
        {
        }
        Bounds(NegIndex n, Index j, NegIndex l)
            : start(std::abs(n))
            , stop(j.isAll() ? max : static_cast<size_t>(j))
            , step(l)
            , reverseStart(n < 0)
            , reverseStop(false)
        {
        }
        Bounds(Index i, NegIndex m, NegIndex l)
            : start(i.isAll() ? 0 : static_cast<size_t>(i))
            , stop(std::abs(m))
            , step(l)
            , reverseStart(false)
            , reverseStop(m < 0)
        {
        }
        Bounds(NegIndex n, NegIndex m, NegIndex l)
            : start(std::abs(n))
            , stop(std::abs(m))
            , step(l)
            , reverseStart(n < 0)
            , reverseStop(m < 0)
        {
        }
    };
public:
    using VBounds = std::vector<Bounds>;
private:
    VBounds m_bounds;

public:
    Slice()
        : Slice({Bounds()})
    {
    }
    Slice(std::vector<Bounds> allBounds)
        : m_bounds(allBounds)
    {
    }

    const VBounds& bounds() const { return m_bounds; }

    class SliceIter
    {
    public:
        using MultiDim = std::vector<size_t>;
        SliceIter(const Slice& slice)
        : m_slice(slice)
        {}
        SliceIter(const Slice& slice, const MultiDim& dimensions)
        : m_slice(slice)
        , m_dimensions(dimensions)
        , current(dimensions.size(), 0)
        {
            // TODO Add exceptions if the length of Slice and dimensions do not match.
        }
//        SliceIter(SliceIter& other);
        SliceIter& operator++();
        SliceIter operator++(int);
        SliceIter& operator--();
        SliceIter operator--(int);
        /*!
         * Returns the one-dimensional index equivalent to the current state of the iterator
         */
        size_t index() const {
            // TODO deal with negative positions
            return Indexer::indexer(m_dimensions, current);
        }
        /*!
         * Sets the state to the beginning of the slice
         */
        SliceIter& toBegin()
        {
            // TODO deal with reverseStart == true
            for (size_t dim = 0; dim < m_slice.bounds().size(); ++dim) {
                current[dim] = m_slice.bounds()[dim].start;
            }
            return *this;
        }
        /*!
         * Sets the state to the end of the slice.
         */
        SliceIter& toEnd()
        {
            // TODO deal with negative indices
            // TODO deal with non-unit steps
            size_t ndim = m_slice.bounds().size();
            for (size_t dim = 0; dim < ndim - 1; ++dim) {
                current[dim] = 0;
            }
            current[ndim - 1] = m_slice.bounds()[ndim - 1].stop;
            return *this;
        }
    private:
        const MultiDim m_dimensions;
        MultiDim current;
        const Slice& m_slice;
    };
};
