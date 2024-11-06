#include <cstddef>
#include <limits>
#include <vector>

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
    std::vector<Bounds> bounds;

public:
    Slice()
        : Slice(Bounds())
    {
    }
    Slice(Bounds bx)
        : bounds({ bx })
    {
    }
    Slice(Bounds bx, Bounds by)
        : bounds({ bx, by })
    {
    }
    Slice(Bounds bx, Bounds by, Bounds bz)
        : bounds({ bx, by, bz })
    {
    }
    Slice(Bounds bx, Bounds by, Bounds bz, Bounds bw)
        : bounds({ bx, by, bz, bw })
    {
    }
    Slice(Bounds bx, Bounds by, Bounds bz, Bounds bw, Bounds bv)
        : bounds({ bx, by, bz, bw, bv })
    {
    }
    Slice(Bounds bx, Bounds by, Bounds bz, Bounds bw, Bounds bv, Bounds bu)
        : bounds({ bx, by, bz, bw, bv, bu })
    {
    }
    Slice(Bounds bx, Bounds by, Bounds bz, Bounds bw, Bounds bv, Bounds bu, Bounds bt)
        : bounds({ bx, by, bz, bw, bv, bu, bt })
    {
    }
    Slice(Bounds bx, Bounds by, Bounds bz, Bounds bw, Bounds bv, Bounds bu, Bounds bt, Bounds bs)
        : bounds({ bx, by, bz, bw, bv, bu, bt, bs })
    {
    }
    // Support up to 8 dimensions as individual arguments
    Slice(std::vector<Bounds> allBounds)
        : bounds(allBounds)
    {
    }

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
        size_t index() const;
        /*!
         * Sets the state to the beginning of the slice
         */
        SliceIter& toBegin()
        {
            // TODO deal with reverseStart == true

            return *this;
        }
        /*!
         * Sets the state to the end of the slice.
         */
        SliceIter& toEnd()
        {
            return *this;
        }
    private:
        const MultiDim m_dimensions;
        MultiDim current;
        const Slice& m_slice;
    };
};
