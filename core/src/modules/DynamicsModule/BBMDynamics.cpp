/*!
 * @file BBMDynamics.cpp
 *
 * @date 11 Nov 2024
 * @author Tim Spain <timothy.spain@nersc.no>
 * @author Einar Ólason <einar.olason@nersc.no>
 */

#include "include/BBMDynamics.hpp"

#include "include/gridNames.hpp"

namespace Nextsim {

static const std::vector<std::string> namedFields = { hiceName, ciceName, uName, vName };
static const std::map<std::string, std::pair<ModelArray::Type, double>> defaultFields = {
    { damageName, { ModelArray::Type::H, 1.0 } },
};

static double compactionParamDefault = -20.; //!< Compation parameter
static double nu0Default = 1. / 3.; //!< \param Poisson's ratio
static double youngDefault = 5.96e8; //!< \param Young's modulus
static double P0Default = 10e3; //! < \param Constant to parametrize Pmax
static double lambda0Default = 1e7; //!< \param lambda
static int alphaDefault = 5;
static double expPMaxDefault = 1.5; //! \param Power of ice thickness in the pressure coefficient
static double muDefault = 0.7; //!< \param tan_phi (double) Internal friction coefficient (mu)
static double comprCapDefault
    = 1e10; //! \param compr_strength (double) Maximum compressive strength [N/m2]
static double cLabDefault = 2e6; //! \param C_lab (double) Test [Pa]
static const int nStepsDefault = 100; //!< Number of sub-steps

// TODO: We should use getName() here, but it isn't static.
static const std::string prefix = "BBMDynamics"; // MEVPDynamics::getName();
static const std::map<int, std::string> keyMap = {
    { BBMDynamics::C_KEY, prefix + ".C" },
    { BBMDynamics::NU_KEY, prefix + ".nu" },
    { BBMDynamics::YOUNG_KEY, prefix + ".young" },
    { BBMDynamics::P0_KEY, prefix + ".P0" },
    { BBMDynamics::LAMBDA0_KEY, prefix + ".lambda0" },
    { BBMDynamics::ALPHA_KEY, prefix + ".alpha" },
    { BBMDynamics::EXPPMAX_KEY, prefix + ".exppmax" },
    { BBMDynamics::MU_KEY, prefix + ".mu" },
    { BBMDynamics::NMAX_KEY, prefix + ".namx" },
    { BBMDynamics::CLAB_KEY, prefix + ".clab" },
    { BBMDynamics::NSTEPS_KEY, prefix + ".nsteps" },
};

void BBMDynamics::configure()
{
    params.setCompactionParam(
        Configured::getConfiguration(keyMap.at(C_KEY), compactionParamDefault));
    params.setNu0(Configured::getConfiguration(keyMap.at(NU_KEY), nu0Default));
    params.setYoung(Configured::getConfiguration(keyMap.at(YOUNG_KEY), youngDefault));
    params.setP0(Configured::getConfiguration(keyMap.at(P0_KEY), P0Default));
    params.setLambda0(Configured::getConfiguration(keyMap.at(LAMBDA0_KEY), lambda0Default));
    params.setAlpha(Configured::getConfiguration(keyMap.at(ALPHA_KEY), alphaDefault));
    params.setExpPMax(Configured::getConfiguration(keyMap.at(EXPPMAX_KEY), expPMaxDefault));
    params.setMu(Configured::getConfiguration(keyMap.at(MU_KEY), muDefault));
    params.setComprCap(Configured::getConfiguration(keyMap.at(NMAX_KEY), comprCapDefault));
    params.setCLab(Configured::getConfiguration(keyMap.at(CLAB_KEY), cLabDefault));
    params.setNSteps(Configured::getConfiguration(keyMap.at(NSTEPS_KEY), nStepsDefault));
}

BBMDynamics::BBMDynamics()
    : IDynamics(true)
    , kernel(params)
{
    getStore().registerArray(Protected::ICE_U, &uice, RO);
    getStore().registerArray(Protected::ICE_V, &vice, RO);
}

void BBMDynamics::setData(const ModelState::DataMap& ms)
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
    // Required data
    for (const auto& fieldName : namedFields) {
        kernel.setData(fieldName, ms.at(fieldName));
    }
    // Data that can have a default value
    for (const auto entry : defaultFields) {
        // Directly add data that is supplied
        const std::string& fieldName = entry.first;
        if (ms.count(fieldName) > 0) {
            kernel.setData(fieldName, ms.at(fieldName));
        } else {
            // Fill data that is not supplied, masking if the mask is available
            ModelArray data(entry.second.first);
            data.resize();
            // Fill the default value
            data = entry.second.second;
            // Mask the default data
            kernel.setData(fieldName, mask(data));
        }
    }
}

void BBMDynamics::update(const TimestepTime& tst)
{
    std::cout << tst.start << std::endl;

    // Fill the updated damage array with the initial value
    damage = damage0.data();

    // set the updated ice thickness, concentration and damage
    kernel.setData(hiceName, hice.data());
    kernel.setData(ciceName, cice.data());
    kernel.setData(damageName, damage);

    // set the forcing velocities
    kernel.setData(uWindName, uwind.data());
    kernel.setData(vWindName, vwind.data());
    kernel.setData(uOceanName, uocean.data());
    kernel.setData(vOceanName, vocean.data());

    /*
     * Ice velocity components are stored in the dynamics, and not changed by the model outside the
     * dynamics kernel. Hence they are not set at this point.
     */

    kernel.update(tst);

    hice.data() = kernel.getDG0Data(hiceName);
    cice.data() = kernel.getDG0Data(ciceName);
    damage = kernel.getDG0Data(damageName);

    uice = kernel.getDG0Data(uName);
    vice = kernel.getDG0Data(vName);
}

// All data for prognostic output
ModelState BBMDynamics::getState() const
{
    // Get the velocities from IDynamics
    ModelState state(IDynamics::getState());

    // Kernel prognostic fields
    state.merge({
        { hiceName, kernel.getDGData(hiceName) },
        { ciceName, kernel.getDGData(ciceName) },
        { damageName, kernel.getDGData(damageName) },
    });

    return state;
}

ModelState BBMDynamics::getStateRecursive(const OutputSpec& os) const
{
    // Base class state
    ModelState state(IDynamics::getStateRecursive(os));

    if (os.allComponents()) {
        state.merge({
            { hiceName, kernel.getDGData(hiceName) },
            { ciceName, kernel.getDGData(ciceName) },
            { damageName, kernel.getDGData(damageName) },
        });
    }
    return state;
}

BBMDynamics::HelpMap& BBMDynamics::getHelpText(HelpMap& map, bool getAll)
{
    map["BBMDynamics"] = {
        { keyMap.at(C_KEY), ConfigType::NUMERIC, { "-∞", "0" },
            std::to_string(compactionParamDefault), "[None]", "The compaction parameter C" },
        { keyMap.at(NU_KEY), ConfigType::NUMERIC, { "-∞", "0" }, std::to_string(nu0Default),
            "[None]", "Poisson's ratio, 𝜈" },
        { keyMap.at(YOUNG_KEY), ConfigType::NUMERIC, { "0", "∞" }, std::to_string(youngDefault),
            "Pa", "Young's modulus, Y" },
        { keyMap.at(P0_KEY), ConfigType::NUMERIC, { "0", "∞" }, std::to_string(P0Default), "Pa",
            "Ice strength scaling parameter" },
        { keyMap.at(LAMBDA0_KEY), ConfigType::NUMERIC, { "0", "∞" }, std::to_string(lambda0Default),
            "s", "Undamaged relaxation time scale" },
        { keyMap.at(ALPHA_KEY), ConfigType::NUMERIC, { "2", "∞" }, std::to_string(alphaDefault),
            "[None]", "Damage parameter" },
        { keyMap.at(EXPPMAX_KEY), ConfigType::NUMERIC, { "0", "2" }, std::to_string(expPMaxDefault),
            "[None]", "Exponent for thickness scaling of P_{max}" },
        { keyMap.at(MU_KEY), ConfigType::NUMERIC, { "0", "1" }, std::to_string(expPMaxDefault),
            "[None]", "Internal friction coefficient, 𝜇" },
        { keyMap.at(NMAX_KEY), ConfigType::NUMERIC, { "0", "∞" }, std::to_string(expPMaxDefault),
            "Pa", "Maximum compressive strength (at the lab scale)" },
        { keyMap.at(CLAB_KEY), ConfigType::NUMERIC, { "0", "∞" }, std::to_string(cLabDefault), "Pa",
            "Cohesion (at the lab scale)" },
        { keyMap.at(NSTEPS_KEY), ConfigType::NUMERIC, { "0", "0.5" }, std::to_string(nStepsDefault),
            "[No unit]", "The number of sub-cycling steps" },
    };
    return map;
}

BBMDynamics::HelpMap& BBMDynamics::getHelpRecursive(HelpMap& map, bool getAll)
{
    return getHelpText(map, getAll);
}

} /* namespace Nextsim */
