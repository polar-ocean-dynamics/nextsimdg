set(COMMON_INCLUDE_DIRS
    "../../core/src/discontinuousgalerkin"
    "../../core/src"
    "../../core/src/modules"
    "../src"
    "../src/modules"
)

set(ModulesRoot "../src/modules")
set(CoreModulesRoot "../../core/src/modules")

# Generate the boundary test data files
execute_process(
    COMMAND "${Python_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/era5_test_data.py"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
execute_process(
    COMMAND "${Python_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/topaz_test_data.py"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

include_directories(${COMMON_INCLUDE_DIRS})

if(ENABLE_MPI)
    add_executable(testERA5Atm_MPI1 "ERA5Atm_test.cpp" "../../core/test/MainMPI.cpp")
    target_compile_definitions(
        testERA5Atm_MPI1
        PRIVATE USE_MPI TEST_FILES_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"
    )
    target_include_directories(testERA5Atm_MPI1 PRIVATE "${ModulesRoot}/AtmosphereBoundaryModule")
    target_link_libraries(testERA5Atm_MPI1 PRIVATE nextsimlib doctest::doctest)

    add_executable(testTOPAZOcn_MPI1 "TOPAZOcn_test.cpp" "../../core/test/MainMPI.cpp")
    target_include_directories(testTOPAZOcn_MPI1 PRIVATE "${ModulesRoot}/OceanBoundaryModule")
    target_compile_definitions(
        testTOPAZOcn_MPI1
        PRIVATE USE_MPI TEST_FILES_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"
    )
    target_link_libraries(testTOPAZOcn_MPI1 PRIVATE nextsimlib doctest::doctest)

    # In the MPI tests it's important to include the "-ns" flag which tells doctest to fail if test has been skipped.
    # Without this, the test will technically pass if we run with fewer ranks than requested.
    add_test(NAME testERA5Atm_MPI1 COMMAND ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} 1 testERA5Atm_MPI1 -ns)
    add_test(NAME testTOPAZOcn_MPI1 COMMAND ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} 1 testTOPAZOcn_MPI1 -ns)
else()
    add_executable(testERA5Atm "ERA5Atm_test.cpp")
    target_compile_definitions(testERA5Atm PRIVATE TEST_FILES_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")
    target_include_directories(testERA5Atm PRIVATE "${ModulesRoot}/AtmosphereBoundaryModule")
    target_link_libraries(testERA5Atm PRIVATE nextsimlib doctest::doctest)

    add_executable(testTOPAZOcn "TOPAZOcn_test.cpp")
    target_include_directories(testTOPAZOcn PRIVATE "${ModulesRoot}/OceanBoundaryModule")
    target_compile_definitions(testTOPAZOcn PRIVATE TEST_FILES_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")
    target_link_libraries(testTOPAZOcn PRIVATE nextsimlib doctest::doctest)

    add_executable(testIceGrowth "IceGrowth_test.cpp")
    target_include_directories(
        testIceGrowth
        PRIVATE "${ModulesRoot}/OceanBoundaryModule" "${CoreModulesRoot}/FreezingPointModule"
    )
    target_link_libraries(testIceGrowth PRIVATE nextsimlib doctest::doctest)

    add_executable(testThermoWinton "ThermoWintonTemperature_test.cpp")
    target_include_directories(
        testThermoWinton
        PRIVATE "${ModulesRoot}/IceThermodynamicsModule" "${ModulesRoot}/OceanBoundaryModule"
    )
    target_link_libraries(testThermoWinton PRIVATE nextsimlib doctest::doctest)

    add_executable(testFEFluxes "FiniteElementFluxes_test.cpp")
    target_include_directories(
        testFEFluxes
        PRIVATE
            "${ModulesRoot}/FluxCalculationModule"
            "${ModulesRoot}/OceanBoundaryModule"
            "${CoreModulesRoot}/FreezingPointModule"
    )
    target_link_libraries(testFEFluxes PRIVATE nextsimlib doctest::doctest)

    add_executable(testBIOHFluxes "BasicIceOceanFlux_test.cpp")
    target_include_directories(
        testBIOHFluxes
        PRIVATE
            "${ModulesRoot}/IceOceanHeatFluxModule"
            "${ModulesRoot}/OceanBoundaryModule"
            "${CoreModulesRoot}/FreezingPointModule"
    )
    target_link_libraries(testBIOHFluxes PRIVATE nextsimlib doctest::doctest)

    add_executable(testSpecHum "SpecificHumidity_test.cpp")
    target_include_directories(testSpecHum PRIVATE "${ModulesRoot}/FluxCalculationModule")
    target_link_libraries(testSpecHum PRIVATE nextsimlib doctest::doctest)

    add_executable(testThermoIce0 "ThermoIce0_test.cpp")
    target_include_directories(testThermoIce0 PRIVATE "${ModulesRoot}/IceThermodynamicsModule")
    target_link_libraries(testThermoIce0 PRIVATE nextsimlib doctest::doctest)

    add_executable(testIceMinima "IceMinima_test.cpp")
    target_link_libraries(testIceMinima PRIVATE nextsimlib doctest::doctest)

    add_executable(testConstantOcn "ConstantOceanBoundary_test.cpp")
    target_include_directories(testConstantOcn PRIVATE "${ModulesRoot}/OceanBoundaryModule")
    target_link_libraries(testConstantOcn PRIVATE nextsimlib doctest::doctest)

    add_executable(testSlabOcn "SlabOcean_test.cpp")
    target_include_directories(testSlabOcn PRIVATE "${CoreModulesRoot}/FreezingPointModule")
    target_link_libraries(testSlabOcn PRIVATE nextsimlib doctest::doctest)

    add_executable(testUniformOcean "UniformOcean_test.cpp")
    target_include_directories(testUniformOcean PRIVATE "${ModulesRoot}/OceanBoundaryModule")
    target_link_libraries(testUniformOcean PRIVATE nextsimlib doctest::doctest)

    add_executable(testBenchmarkBoundaries "BenchmarkBoundaries_test.cpp")
    target_include_directories(
        testBenchmarkBoundaries
        PRIVATE "${ModulesRoot}/AtmosphereBoundaryModule" "${ModulesRoot}/OceanBoundaryModule"
    )
    target_link_libraries(testBenchmarkBoundaries PRIVATE nextsimlib doctest::doctest)

    add_executable(testDamageHealing "DamageHealing_test.cpp")
    target_include_directories(
        testDamageHealing
        PRIVATE "${ModulesRoot}/DamageHealingModule" "${ModulesRoot}/DamageHealingModule"
    )
    target_link_libraries(testDamageHealing PRIVATE nextsimlib doctest::doctest)

    add_test(NAME testERA5Atm COMMAND testERA5Atm)
    add_test(NAME testTOPAZOcn COMMAND testTOPAZOcn)
    add_test(NAME testIceGrowth COMMAND testIceGrowth)
    add_test(NAME testThermoWinton COMMAND testThermoWinton)
    add_test(NAME testFEFluxes COMMAND testFEFluxes)
    add_test(NAME testBIOHFluxes COMMAND testBIOHFluxes)
    add_test(NAME testSpecHum COMMAND testSpecHum)
    add_test(NAME testThermoIce0 COMMAND testThermoIce0)
    add_test(NAME testIceMinima COMMAND testIceMinima)
    add_test(NAME testConstantOcn COMMAND testConstantOcn)
    add_test(NAME testSlabOcn COMMAND testSlabOcn)
    add_test(NAME testUniformOcean COMMAND testUniformOcean)
    add_test(NAME testBenchmarkBoundaries COMMAND testBenchmarkBoundaries)
    add_test(NAME testDamageHealing COMMAND testDamageHealing)
endif()
