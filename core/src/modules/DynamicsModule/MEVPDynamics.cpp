/*!
 * @file MEVPDynamics.cpp
 *
 * @date 11 Nov 2024
 * @author Tim Spain <timothy.spain@nersc.no>
 * @author Piotr Minakowski <piotr.minakowski@ovgu.de>
 * @author Einar Ólason <einar.olason@nersc.no>
 */

#include "include/MEVPDynamics.hpp"

#include "include/gridNames.hpp"

#include <string>
#include <vector>

namespace Nextsim {

// Degrees to radians as a hex float
static const double radians = 0x1.1df46a2529d39p-6;

static const double compactionParamDefault
    = -20.; //!< Compation parameter: Hibler's C in exp(-C(1-a))
static const double pStarDefault = 27.5e3; //!< Ice strength
static const double deltaMinDefault = 2e-9; //!< Viscous regime
static const int nStepsDefault = 100; //!< Number of sub-steps

// TODO: We should use getName() here, but it isn't static.
static const std::string prefix = "MEVPDynamics"; // MEVPDynamics::getName();
static const std::map<int, std::string> keyMap = {
    { MEVPDynamics::PSTAR_KEY, prefix + ".Pstar" },
    { MEVPDynamics::DELTA_KEY, prefix + ".DeltaMin" },
    { MEVPDynamics::C_KEY, prefix + ".C" },
    { MEVPDynamics::NSTEPS_KEY, prefix + ".nsteps" },
};

void MEVPDynamics::configure()
{
    Module::Module<Nextsim::IDamageHealing>::setImplementation("Nextsim::NoHealing");

    params.setPStar(Configured::getConfiguration(keyMap.at(PSTAR_KEY), pStarDefault));
    params.setDeltaMin(Configured::getConfiguration(keyMap.at(DELTA_KEY), deltaMinDefault));
    params.setCompactionParam(
        Configured::getConfiguration(keyMap.at(C_KEY), compactionParamDefault));
    params.setNSteps(Configured::getConfiguration(keyMap.at(NSTEPS_KEY), nStepsDefault));
}

static const std::vector<std::string> namedFields = { hiceName, ciceName, uName, vName };
MEVPDynamics::MEVPDynamics()
    : IDynamics()
    , kernel(params)
{
    getStore().registerArray(Protected::ICE_U, &uice, RO);
    getStore().registerArray(Protected::ICE_V, &vice, RO);
}

void MEVPDynamics::setData(const ModelState::DataMap& ms)
{
    IDynamics::setData(ms);

    bool isSpherical = checkSpherical(ms);

    ModelArray coords = ms.at(coordsName);
    if (isSpherical) {
        coords *= radians;
    }
    // TODO: Some encoding of the periodic edge boundary conditions
    kernel.initialise(coords, isSpherical, ms.at(maskName));

    uice = ms.at(uName);
    vice = ms.at(vName);

    // Set the data in the kernel arrays.
    for (const auto& fieldName : namedFields) {
        kernel.setData(fieldName, ms.at(fieldName));
    }
}

void MEVPDynamics::update(const TimestepTime& tst)
{
    std::cout << tst.start << std::endl;

    // set the updated ice thickness and concentration
    kernel.setData(hiceName, hice.data());
    kernel.setData(ciceName, cice.data());

    // set the forcing velocities
    kernel.setData(uWindName, uwind.data());
    kernel.setData(vWindName, vwind.data());
    kernel.setData(uOceanName, uocean.data());
    kernel.setData(vOceanName, vocean.data());

    // kernel.setData(uName, uice);
    // kernel.setData(vName, vice);

    kernel.update(tst);

    hice.data() = kernel.getDG0Data(hiceName);
    cice.data() = kernel.getDG0Data(ciceName);

    uice = kernel.getDG0Data(uName);
    vice = kernel.getDG0Data(vName);
}

ModelState MEVPDynamics::getStateRecursive(const OutputSpec& os) const
{
    // Base class state
    ModelState state(IDynamics::getStateRecursive(os));

    if (os.allComponents()) {
        state.merge({
            { hiceName, kernel.getDG0Data(hiceName) },
            { ciceName, kernel.getDG0Data(ciceName) },
        });
    }
    return state;
}

MEVPDynamics::HelpMap& MEVPDynamics::getHelpText(HelpMap& map, bool getAll)
{
    map["MEVPDynamics"] = {
        { keyMap.at(PSTAR_KEY), ConfigType::NUMERIC, { "0", "∞" }, std::to_string(pStarDefault),
            "Pa", "The sea-ice strength parameter P*" },
        { keyMap.at(DELTA_KEY), ConfigType::NUMERIC, { "0", "∞" }, std::to_string(deltaMinDefault),
            "1/s", "Capping value of the 𝛥 variable" },
        { keyMap.at(C_KEY), ConfigType::NUMERIC, { "-∞", "0" },
            std::to_string(compactionParamDefault), "[None]", "The compaction parameter C" },
        { keyMap.at(NSTEPS_KEY), ConfigType::NUMERIC, { "0", "∞" }, std::to_string(nStepsDefault),
            "[No unit]", "The number of sub-cycling steps" },
    };
    return map;
}

MEVPDynamics::HelpMap& MEVPDynamics::getHelpRecursive(HelpMap& map, bool getAll)
{
    return getHelpText(map, getAll);
}

}
