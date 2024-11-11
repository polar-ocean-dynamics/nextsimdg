/*!
 * @file BBMParameters.hpp
 * @date 11 Nov 2024
 * @author Tim Spain <timothy.spain@nersc.no>
 * @author Piotr Minakowski <piotr.minakowski@ovgu.de>
 */

#ifndef __MEBPARAMETERS_HPP
#define __MEBPARAMETERS_HPP

#include "DynamicsParameters.hpp"

namespace Nextsim {

class BBMParameters : public DynamicsParameters {
    double compactionParam = -20.; //!< Compation parameter
    double nu0 = 1. / 3.; //!< \param Poisson's ratio
    double young = 5.96e8; //!< \param Young's modulus
    double P0 = 10e3; //! < \param Constant to parametrize Pmax
    double lambda0 = 1e7; //!< \param lambda
    int alpha = 5;
    double expPMax = 1.5; //! \param Power of ice thickness in the pressure coefficient
    double mu = 0.7; //!< \param tan_phi (double) Internal friction coefficient (mu)
    double comprCap = 1e10; //! \param compr_strength (double) Maximum compressive strength [N/m2]
    double cLab = 2e6; //! \param C_lab (double) Test [Pa]
    int nSteps = 120; //!< number of sub-cycling steps

public:
    BBMParameters() = default;

    void setCompactionParam(const double C) { compactionParam = C; };
    void setNu0(const double n) { nu0 = n; };
    void setYoung(const double Y) { young = Y; };
    void setP0(const double P) { P0 = P; };
    void setLambda0(const double l) { lambda0 = l; };
    void setAlpha(const double a) { alpha = a; };
    void setExpPMax(const double epm) { expPMax = epm; };
    void setMu(const double m) { mu = m; };
    void setComprCap(const double cc) { comprCap = cc; };
    void setCLab(const double cl) { cLab = cl; };
    void setNSteps(const int N) { nSteps = N; };

    double getCompactionParam() const { return compactionParam; };
    double getNu0() const { return nu0; };
    double getYoung() const { return young; };
    double getP0() const { return P0; };
    double getLambda0() const { return lambda0; };
    double getAlpha() const { return alpha; };
    double getExpPMax() const { return expPMax; };
    double getMu() const { return mu; };
    double getComprCap() const { return comprCap; };
    double getCLab() const { return cLab; };
    const int getNSteps() const { return nSteps; }

    double c0 = 10e3; //! \param
};

} /* namespace Nextsim */

#endif /* __MEB_HPP */
