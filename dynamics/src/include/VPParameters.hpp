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

private:
    double compactionParam = -20.; //!< Compation parameter: Hibler's C in exp(-C(1-a))
    double pStar = 27.5e3; //!< Ice strength
    double deltaMin = 2e-9; //!< Viscous regime

public:
    VPParameters() = default;

    void setCompactionParam(const double C) { compactionParam = C; };
    void setPstar(const double P) { pStar = P; };
    void setDeltaMin(const double D) { deltaMin = D; };

    double getCompactionParam() const { return compactionParam; }
    double getPStar() const { return pStar; }
    double getDeltaMin() const { return deltaMin; }
};

} /* namespace Nextsim */

#endif /* __VP_HPP */
