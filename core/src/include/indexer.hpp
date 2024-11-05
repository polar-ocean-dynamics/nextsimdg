/*!
 * @file indexer.hpp
 *
 * @date Nov 5, 2024
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#ifndef INDEXER_HPP
#define INDEXER_HPP

#include <cstddef>
#include <vector>
#include <limits>

namespace Indexer {

template <typename T = size_t, typename D = std::vector<T>, typename L = D> T indexer(const D& dims, const L& loc)
{
    auto iloc = std::begin(loc);
    auto idim = std::begin(dims);
    T stride = 1;
    T index = 0;
    do {
        index += stride * *iloc;
        stride *= *idim;

    } while (++iloc != std::end(loc) && ++idim != std::end(dims));
    return index;
}
} /* namespace Indexer */
#endif /* INDEXER_HPP */
