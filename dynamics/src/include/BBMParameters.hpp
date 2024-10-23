/*!
 * @file BBMParameters.hpp
 * @date 09 Nov 2024
 * @author Tim Spain <timothy.spain@nersc.no>
 * @author Piotr Minakowski <piotr.minakowski@ovgu.de>
 */

#ifndef __MEBPARAMETERS_HPP
#define __MEBPARAMETERS_HPP

#include "DynamicsParameters.hpp"

namespace Nextsim {

class BBMParameters : public DynamicsParameters {
public:
    double compaction_param = -20.; //!< Compation parameter
    double nu0 = 1. / 3.; //!< \param Poisson's ratio
    double young = 5.96e8; //!< \param Young's modulus
    double P0 = 10e3; //! < \param Constant to parametrize Pmax
    double undamaged_time_relaxation_sigma = 1e7; //!< \param lambda
    int exponent_relaxation_sigma = 5;
    double c0 = 10e3; //! \param

    double exponent_compression_factor
        = 1.5; //! \param Power of ice thickness in the pressure coefficient

    // TODO missing 45\deg it goes to Compresssion
    double tan_phi = 0.7; //!< \param tan_phi (double) Internal friction coefficient (mu)

    double compr_strength
        = 1e10; //! \param compr_strength (double) Maximum compressive strength [N/m2]
    double C_lab = 2e6; //! \param C_lab (double) Test [Pa]

    BBMParameters() = default;
};

} /* namespace Nextsim */

#endif /* __MEB_HPP */
