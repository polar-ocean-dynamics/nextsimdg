/*!
 * @file DynamicsParameters.hpp
 *
 * @date 09 Nov 2024
 * @author Tim Spain <timothy.spain@nersc.no>
 * @author Thomas Richter <thomas.richter@ovgu.de>
 *
 */

#ifndef DYNAMICSPARAMETERS_HPP
#define DYNAMICSPARAMETERS_HPP

namespace Nextsim {
class DynamicsParameters {
public:
    double rho_ice = 900.; //!< Sea ice density
    double rho_atm = 1.3; //!< Air density
    double rho_ocean = 1026.; //!< Ocean density

    double C_atm = 1.2e-3; //!< Air drag coefficient
    double C_ocean = 5.5e-3; //!< Ocean drag coefficient

    double F_atm = C_atm * rho_atm; //!< effective factor for atm-forcing
    double F_ocean = C_ocean * rho_ocean; //!< effective factor for ocean-forcing

    double fc = 1.45842e-4; //!< Coriolis

    double ocean_turning_angle = 25.; //!< Ocean turning angle

    double gravity = 9.81; //!< gravity parameter

    DynamicsParameters() = default;
};
}

#endif /* DYNAMICSPARAMETERS_HPP */
