/*!
 * @file    XiosFile_test.cpp
 * @author  Joe Wallwork <jw2423@cam.ac.uk>
 * @date    30 Apr 2025
 * @brief   Tests for XIOS axes
 * @details
 * This test is designed to test axis functionality of the C++ interface
 * for XIOS.
 *
 */
#include <doctest/extensions/doctest_mpi.h>
#undef INFO

#include "StructureModule/include/ParametricGrid.hpp"
#include "include/Xios.hpp"

using namespace doctest;

const std::string testSourceDir = TEST_SOURCE_DIR;
const std::string configFileName = testSourceDir + "/xios_tests.cfg";

namespace Nextsim {

/*!
 * TestXiosInitialization
 *
 * This function tests the file functionality of the C++ interface for XIOS. It
 * needs to be run with 2 ranks i.e.,
 *
 * `mpirun -n 2 ./testXiosFile_MPI2`
 *
 */
MPI_TEST_CASE("TestXiosFile", 2)
{
    enableXios(configFileName);

    // Initialize an Xios instance called xios_handler
    Xios xios_handler;
    REQUIRE(xios_handler.isInitialized());
    const size_t size = xios_handler.getClientMPISize();
    REQUIRE(size == 2);
    const size_t rank = xios_handler.getClientMPIRank();

    // Create a simple axis with two points
    xios_handler.createAxis("axis_A");
    xios_handler.setAxisValues("axis_A", { 0.0, 1.0 });

    // Create a 1D grid comprised of the single axis
    xios_handler.createGrid("grid_1D");
    xios_handler.gridAddAxis("grid_1D", "axis_A");

    // Create a field on the 1D grid
    xios_handler.createField("field_A");
    xios_handler.setFieldOperation("field_A", "instant");
    xios_handler.setFieldGridRef("field_A", "grid_1D");
    xios_handler.setFieldReadAccess("field_A", false);

    // --- Tests for file API
    const std::string fileId = "output";
    REQUIRE_THROWS_WITH(xios_handler.getFileName(fileId), "Xios: Undefined file 'output'");
    xios_handler.createFile(fileId);
    REQUIRE_THROWS_WITH(xios_handler.createFile(fileId), "Xios: File 'output' already exists");
    // File name
    // NOTE: This is read from the XiosOutput.period entry in xios_tests.cfg when a field is added
    // to be written (i.e., readAccess=false)
    REQUIRE_THROWS_WITH(xios_handler.getFileName(fileId), "Xios: Undefined name for file 'output'");
    {
        // Add field
        xios_handler.fileAddField(fileId, "field_A");
        std::vector<std::string> fieldIds = xios_handler.fileGetFieldIds(fileId);
        REQUIRE(fieldIds.size() == 1);
        REQUIRE(fieldIds[0] == "field_A");
    }
    REQUIRE(xios_handler.getFileName(fileId) == "xios_test_output");
    const std::string fileName = "diagnostic";
    xios_handler.setFileName(fileId, fileName);
    REQUIRE(xios_handler.getFileName(fileId) == fileName);
    // File type
    REQUIRE_THROWS_WITH(xios_handler.getFileType(fileId), "Xios: Undefined type for file 'output'");
    const std::string fileType = "one_file";
    xios_handler.setFileType(fileId, fileType);
    REQUIRE(xios_handler.getFileType(fileId) == fileType);
    // Output frequency
    // NOTE: This is read from the XiosOutput.period entry in xios_tests.cfg upon file creation
    REQUIRE(xios_handler.getFileOutputFreq(fileId).seconds() == 3.0 * 60 * 60);
    Duration timestep = xios_handler.getCalendarTimestep();
    xios_handler.setFileOutputFreq(fileId, timestep);
    REQUIRE(xios_handler.getFileOutputFreq(fileId).seconds() == 1.5 * 60 * 60);
    // Split frequency
    REQUIRE_THROWS_WITH(
        xios_handler.getFileSplitFreq(fileId), "Xios: Undefined split frequency for file 'output'");
    xios_handler.setFileSplitFreq(fileId, timestep);
    REQUIRE(xios_handler.getFileSplitFreq(fileId).seconds() == 1.5 * 60 * 60);
    // File mode
    const std::string mode = "write";
    xios_handler.setFileMode(fileId, mode);
    REQUIRE(xios_handler.getFileMode(fileId) == mode);
    // File parallel access mode
    const std::string parAccess = "collective";
    xios_handler.setFileParAccess(fileId, parAccess);
    REQUIRE(xios_handler.getFileParAccess(fileId) == parAccess);

    // Create a new file for each time unit to check more thoroughly that XIOS interprets output
    // frequency and split frequency correctly.
    // (If we reused the same file then the XIOS interface would raise warnings.)
    xios_handler.createFile("year");
    xios_handler.setFileOutputFreq("year", Duration("P1-0T00:00:00"));
    xios_handler.setFileSplitFreq("year", Duration("P2-0T00:00:00"));
    REQUIRE(xios_handler.getFileOutputFreq("year").seconds() == 365 * 24 * 60 * 60);
    REQUIRE(xios_handler.getFileSplitFreq("year").seconds() == 2 * 365 * 24 * 60 * 60);
    xios_handler.createFile("day");
    xios_handler.setFileOutputFreq("day", Duration("P0-1T00:00:00"));
    xios_handler.setFileSplitFreq("day", Duration("P0-2T00:00:00"));
    REQUIRE(xios_handler.getFileOutputFreq("day").seconds() == 24 * 60 * 60);
    REQUIRE(xios_handler.getFileSplitFreq("day").seconds() == 2 * 24 * 60 * 60);
    xios_handler.createFile("hour");
    xios_handler.setFileOutputFreq("hour", Duration("P0-0T01:00:00"));
    xios_handler.setFileSplitFreq("hour", Duration("P0-0T02:00:00"));
    REQUIRE(xios_handler.getFileOutputFreq("hour").seconds() == 60 * 60);
    REQUIRE(xios_handler.getFileSplitFreq("hour").seconds() == 2 * 60 * 60);
    xios_handler.createFile("minute");
    xios_handler.setFileOutputFreq("minute", Duration("P0-0T00:01:00"));
    xios_handler.setFileSplitFreq("minute", Duration("P0-0T00:02:00"));
    REQUIRE(xios_handler.getFileOutputFreq("minute").seconds() == 60);
    REQUIRE(xios_handler.getFileSplitFreq("minute").seconds() == 2 * 60);
    xios_handler.createFile("second");
    xios_handler.setFileOutputFreq("second", Duration("P0-0T00:00:01"));
    xios_handler.setFileSplitFreq("second", Duration("P0-0T00:00:02"));
    REQUIRE(xios_handler.getFileOutputFreq("second").seconds() == 1);
    REQUIRE(xios_handler.getFileSplitFreq("second").seconds() == 2);

    xios_handler.close_context_definition();
    xios_handler.context_finalize();
}
}
