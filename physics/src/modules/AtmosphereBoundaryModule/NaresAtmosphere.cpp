/*!
 * @file NaresAtmosphere.cpp
 *
 * @date 28 Jun 2025
 * @author Lexi Arlen <arlenlex@stanford.edu>
 */

#include "include/NaresAtmosphere.hpp"
#include "include/BenchmarkCoordinates.hpp"
#include "include/Time.hpp"

namespace Nextsim {

void NaresAtmosphere::setData(const ModelState::DataMap& ms)
{
    IAtmosphereBoundary::setData(ms);
    BenchmarkCoordinates::setData();
    // Constant, zero fluxes in the atmosphere
    qia = 0.;
    dqia_dt = 0.;
    qow = 0.;
    subl = 0.;
    snow = 0.;
    rain = 0.;
    evap = 0.;
}

void NaresAtmosphere::update(const TimestepTime& tst)
{
    IAtmosphereBoundary::update(tst);

    // Initialize t0 on first call
    if (!t0Set) {
        t0      = tst.start;
        t0Set   = true;
    }

    // seconds in one day
    constexpr double oneday = 24.0 * 3600.0;
    // target wind speed
    constexpr double vMax   = 22.0;

    // elapsed time since t0, in seconds
    Duration      elapsed   = tst.start - t0;
    double        tsec      = elapsed.seconds();

    // compute current wind speed: linear ramp up to vMax over 1 day;
    // thereafter hold at vMax
    double v;
    if (tsec < 0) {
        v = 0.0;
    } else if (tsec < oneday) {
        v = vMax * (tsec / oneday);
    } else {
        v = vMax;
    }

    const size_t ny = BenchmarkCoordinates::ny();
    const size_t nx = BenchmarkCoordinates::nx();

    // assign uniform u‐wind = v, v‐wind = 0 everywhere
    for (size_t j = 0; j < ny; ++j) {
        for (size_t i = 0; i < nx; ++i) {
            uwind(i, j) = v;
            vwind(i, j) = 0.0;
        }
    }
}
} /* namespace Nextsim */
