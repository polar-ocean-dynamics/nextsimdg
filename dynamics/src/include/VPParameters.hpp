/*!
 * @file VPParameters.hpp
 * @date 09 Nov 2024
 * @author Tim Spain <timothy.spain@nersc.no>
 * @author Thomas Richter <thomas.richter@ovgu.de>
 */

#ifndef __VPPARAMETERS_HPP
#define __VPPARAMETERS_HPP

#include "DynamicsParameters.hpp"

namespace Nextsim {

class VPParameters : public DynamicsParameters {
public:
    double compactionParam = -20.; //!< Compation parameter: Hibler's C in exp(-C(1-a))
    double Pstar = 27.5e3; //!< Ice strength
    double DeltaMin = 2e-9; //!< Viscous regime

    VPParameters() = default;
};

} /* namespace Nextsim */

#endif /* __VP_HPP */
