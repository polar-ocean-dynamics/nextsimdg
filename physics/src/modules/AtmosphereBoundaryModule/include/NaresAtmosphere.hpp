/*!
 * @file NaresAtmosphere.hpp
 *
 * @date 28 Jun 2025
 * @author Lexi Arlen <arlenlex@stanford.edu>
 */

#ifndef NARESATMOSPHERE_HPP
#define NARESATMOSPHERE_HPP

#include "include/IAtmosphereBoundary.hpp"

namespace Nextsim {

class NaresAtmosphere : public IAtmosphereBoundary {
public:
    NaresAtmosphere()
        : IAtmosphereBoundary()
    {
    }
    ~NaresAtmosphere() = default;

    void setData(const ModelState::DataMap&) override;
    std::string getName() const override { return "NaresAtmosphere"; }

    void update(const TimestepTime& tst) override;

private:
    TimePoint t0;
    bool t0Set = false;
};

} /* namespace Nextsim */

#endif /* NARESATMOSPHERE_HPP */
