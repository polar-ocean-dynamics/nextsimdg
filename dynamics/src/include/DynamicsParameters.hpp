/*!
 * @file DynamicsParameters.hpp
 *
 * @date 11 Nov 2024
 * @author Tim Spain <timothy.spain@nersc.no>
 * @author Thomas Richter <thomas.richter@ovgu.de>
 *
 */

#ifndef DYNAMICSPARAMETERS_HPP
#define DYNAMICSPARAMETERS_HPP

namespace Nextsim {
class DynamicsParameters {
private:
    double rhoIce = 900.; //!< Sea ice density
    double rhoAtm = 1.3; //!< Air density
    double rhoOcean = 1026.; //!< Ocean density

    double CAtm = 1.2e-3; //!< Air drag coefficient
    double COcean = 5.5e-3; //!< Ocean drag coefficient

    double fc = 1.45842e-4; //!< Coriolis

    double oceanTurningAngle = 25.; //!< Ocean turning angle

public:
    DynamicsParameters() = default;

    void setRhoIce(const double r) { rhoIce = r; };
    void setRhoAtm(const double r) { rhoAtm = r; };
    void setRhoOcean(const double r) { rhoOcean = r; };
    void setCAtm(const double C) { CAtm = C; };
    void setCOcean(const double C) { COcean = C; };
    void setFC(const double f) { fc = f; };
    void setOceanTurningAngle(const double theta) { oceanTurningAngle = theta; };

    double getRhoIce() const { return rhoIce; };
    double getFAtm() const { return CAtm * rhoAtm; };
    double getFOcean() const { return COcean * rhoOcean; };
    double getFC() const { return fc; };
    double getOceanTurningAngle() const { return oceanTurningAngle; };
};
}

#endif /* DYNAMICSPARAMETERS_HPP */
