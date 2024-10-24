/*!
 * @file VPParameters.hpp
 * @date 11 Nov 2024
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
    int nSteps = 100; //!< number of sub-cycling steps

public:
    VPParameters() = default;

    void setCompactionParam(const double C) { compactionParam = C; };
    void setPStar(const double P) { pStar = P; };
    void setDeltaMin(const double D) { deltaMin = D; };
    void setNSteps(const int N) { nSteps = N; };

    const double getCompactionParam() const { return compactionParam; }
    const double getPStar() const { return pStar; }
    const double getDeltaMin() const { return deltaMin; }
    const int getNSteps() const { return nSteps; }
};

} /* namespace Nextsim */

#endif /* __VP_HPP */
