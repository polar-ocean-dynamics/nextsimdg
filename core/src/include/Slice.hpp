#include <cstddef>
#include <limits>
#include <vector>

#include "include/indexer.hpp"

#include <iostream> // FIXME remove me

// A macro definition of the two-argument ceiling function
#define ceil(num , denom) (((num) + (denom) - 1) / (denom))

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
            : Bounds(0, {}, 1)
        {
        }
        Bounds(Index i)
            : Bounds(i, i+1, 1)
        {
        }
        Bounds(Index i, Index j)
            : Bounds(i, j, 1)
//        : start(i)
//        , stop(j)
        {
        }
        Bounds(Index i, Index j, Int step_in)
            : start(i)
            , stop(j)
            , step(step_in)
        {
        }
    };
public:
    using VBounds = std::vector<Bounds>;
    const VBounds bounds;

public:
    Slice()
        : Slice({Bounds()})
    {
    }
    Slice(std::vector<Bounds> allBounds)
        : bounds(allBounds)
    {
    }

    const size_t n() const { return bounds.size(); }

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
            return incrementDim(0);
        }
        SliceIter operator++(int)
        {
            SliceIter copy(*this);
            ++(*this);
            return copy;
        }

        SliceIter& incrementDim(size_t dim)
        {
            for(;;) {
                // Increment current in this dimension, and test if it is at or past the end.
                if (!stopTest(current[dim] += m_slice.bounds[dim].step, dim)) break;
                // Never reset the final valid dimension.
                if (dim + 1 >= current.size()) break;
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
        Int index() const {
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
                current[dim] = m_slice.bounds[dim].start;
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
            // TODO handle reversed starts, stops, steps
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
            for (size_t dim = 0; dim < m_slice.bounds.size(); ++dim)
            {
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
            return (m_slice.bounds[dim].start.isAll()) ?
                    ((step(dim) < 0) ? m_dimensions[dim] - 1 : 0): static_cast<size_t>(m_slice.bounds[dim].start);
        }
        /*!
         * Translates the default and negative bounds into an actual start
         * index for the first dimension.
         */
        Int start() const
        {
            if (m_slice.n() != 1) {
                throw std::out_of_range("Slice::start(): use Slice::start(size_t) for multi-dimensional slices.");
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
            size_t stop = (m_slice.bounds[dim].stop.isAll()) ? m_dimensions[dim] : static_cast<size_t>(m_slice.bounds[dim].stop);
            return ceil(stop - start(dim), step(dim));
        }
        /*!
         * Calculates the number of elements in the the slice along the first dimension.
         */
        Int nElements() const
        {
            if (m_slice.n() != 1) {
                throw std::out_of_range("Slice::nElements(): use Slice::nElements(size_t) for multi-dimensional slices.");
            }
            return nElements(0);
        }

        Int step(size_t dim) const
        {
            return m_slice.bounds[dim].step;
        }
        Int step() const
        {
            if (m_slice.n() != 1) {
                throw std::out_of_range("Slice::step(): use Slice::step(size_t) for multi-dimensional slices.");
            }
            return step(0);
        }
    private:

        Int dimEnd(size_t dim) const
        {
            return (m_slice.bounds[dim].stop.isAll()) ?
                                m_dimensions[dim] :
                                static_cast<Slice::Int>(m_slice.bounds[dim].stop);
        }
        // Test whether a dimension has run past the end of its bounds
        bool stopTest(Int subject, size_t dim) const
        {
            return subject >= dimEnd(dim);
        }
        bool stopTest(const MultiDim& loc, size_t dim) const { return stopTest(loc[dim], dim); }

        const MultiDim m_dimensions;
        MultiDim current;
        const Slice& m_slice;
    };
};

#undef ceil
