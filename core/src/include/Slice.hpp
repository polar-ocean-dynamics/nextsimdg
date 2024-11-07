#include <cstddef>
#include <limits>
#include <vector>

#include "include/indexer.hpp"

#include <iostream> // FIXME remove me

// Generic n-dimensional slice
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
//                std::cout << "Index::Index()" << std::endl;
            }
            // A constructor only for unsigned integers
            Index(Int ii)
                : m_isAll(false)
                , i(ii)
            {
//                std::cout << "Index::Index(" << ii << ")" << std::endl;
            }
            bool isAll() const { return m_isAll; }
            operator std::ptrdiff_t() const { return i; }

        private:
            bool m_isAll;
            Int i;
        };

    public:
        Index start;
        Index stop;
        Int step;
        Bounds()
            : Bounds(0, {}/*, 1*/)
        {
        }
        Bounds(Index i)
            : Bounds(i, i+1/*, 1*/)
        {
        }
        Bounds(Index i, Index j)
//            : Bounds(i, j, 1)
        : start(i)
        , stop(j)
        {
        }
//        Bounds(Index i, Index j, Int step_in)
//            : start(i)
//            , stop(j)
//            , step(step_in)
//        {
//        }
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
    const size_t n() const { return m_bounds.size(); }

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
            toBegin();
            // TODO Add exceptions if the length of Slice and dimensions do not match.
            if (slice.n() != dimensions.size())
                throw std::invalid_argument("SliceIter: mismatch in number of dimensions between Slice (" + std::to_string(slice.n()) + ") and extent (" + std::to_string(dimensions.size()) + ").");
        }
//        SliceIter(SliceIter& other);
        SliceIter& operator++()
        {
            // TODO deal with non-unit steps
            // TODO deal with multiple dimensions
            size_t dim = 0;
            for(;;) {
                // Increment current in this dimension, and test if it is at or past the end.
                if (!stopTest(++current[dim], dim)) break;
                // Never reset the final valid dimension.
                if (dim + 1 >= current.size()) break;
                // Reset the dimension to the start
                current[dim] = m_slice.bounds()[dim].start;
                // Increment dimension
                ++dim;
            }
            return *this;
        }
        SliceIter operator++(int)
        {
            SliceIter copy(*this);
            ++(*this);
            return copy;
        }
        SliceIter& operator--();
        SliceIter operator--(int);
        /*!
         * Returns the one-dimensional index equivalent to the current state of the iterator
         */
        size_t index() const {
            // TODO deal with negative positions
            if (false) {
                std::cout << "current=(";
                for (auto pos : current) {
                    std::cout << pos << ",";
                }
                std::cout << ")" << std::endl;
            }
            return Indexer::indexer(m_dimensions, current);
        }
        /*!
         * Sets the state to the beginning of the slice
         */
        SliceIter& toBegin()
        {
            // TODO deal with reverseStart == true
            for (size_t dim = 0; dim < m_slice.n(); ++dim) {
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
            size_t lastDim = m_slice.n() - 1;
            for (size_t dim = 0; dim < lastDim - 1; ++dim) {
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
                if (current[dim] != m_slice.bounds()[dim].start)
                    return false;
            }
            return true;
        }
        /*!
         * Is the iterator at its 'end'?
         */
        bool isEnd() const
        {
            // TODO handle reversed starts, stops, steps
            size_t lastDim = m_slice.n() - 1;
            return stopTest(current, lastDim);
        }
    private:

        size_t dimEnd(size_t dim) const
        {
            return (m_slice.bounds()[dim].stop.isAll()) ?
                                m_dimensions[dim] :
                                static_cast<Slice::Int>(m_slice.bounds()[dim].stop);
        }
        // Test whether a dimension has run past the end of its bounds
        bool stopTest(size_t subject, size_t dim) const
        {
            return subject >= dimEnd(dim);
        }
        bool stopTest(const MultiDim& loc, size_t dim) const { return stopTest(loc[dim], dim); }

        const MultiDim m_dimensions;
        MultiDim current;
        const Slice& m_slice;
    };
};
