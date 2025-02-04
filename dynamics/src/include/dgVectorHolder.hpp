/*!
 * @file dgVectorHolder.hpp
 *
 * @date Feb 4, 2025
 * @author Tim Spain <timothy.spain@nersc.no>
 */

#ifndef DGVECTORHOLDER_HPP
#define DGVECTORHOLDER_HPP

#include "dgVector.hpp"
#include "include/ModelArray.hpp"

namespace Nextsim {

template <int DG>
class DGVectorHolder {
public:
    using EigenDGVector = typename DGVector<DG>::EigenDGVector;
    DGVectorHolder(ModelArray& ma)
        : ref(reinterpret_cast<EigenDGVector&>(static_cast<ModelArray::DataType&>(ma)))
    {
    }
    DGVectorHolder(EigenDGVector& edgv)
        :ref(edgv)
    {
    }
    DGVectorHolder(DGVector<DG>& dgv)
        :ref(dgv)
    {
    }

    operator DGVector<DG>()
    {
        return ref;
    }
    operator const DGVector<DG>() const
    {
        return ref;
    }

    double& operator()(size_t i, size_t j)
    {
        return ref(i, j);
    }
private:
    EigenDGVector& ref;
};
}

#endif /* DGVECTORHOLDER_HPP */
