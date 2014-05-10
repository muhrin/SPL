/*
 * CastepGeomOptimiser.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "spl/potential/CastepGeomOptimiser.h"

#include <fstream>
#include <sstream>

#include <boost/algorithm/string/find.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "spl/io/Parsing.h"
#include "spl/io/BoostFilesystem.h"
#include "spl/os/Process.h"
#include "spl/potential/OptimisationSettings.h"

// NAMESPACES ////////////////////////////////
namespace spl {
namespace potential {

namespace fs = boost::filesystem;

CastepGeomOptimiseSettings::CastepGeomOptimiseSettings()
{
  numRoughSteps = 0;
  numConsistentRelaxations = 2;
  keepIntermediateFiles = false;
}

CastepGeomOptimiser::CastepGeomOptimiser(const std::string & castepExe,
    const std::string & castepSeed) :
    myCastepSeed(castepSeed), myCellReaderWriter(), myCastepReader(), myCastepExe(
        castepExe)
{
}

CastepGeomOptimiser::CastepGeomOptimiser(const std::string & castepExe,
    const std::string & castepSeed, const Settings & settings) :
    mySettings(settings), myCastepSeed(castepSeed), myCellReaderWriter(), myCastepReader(), myCastepExe(
        castepExe)
{
}

OptimisationOutcome
CastepGeomOptimiser::optimise(common::Structure & structure,
    const OptimisationSettings & options) const
{
  OptimisationData optimisationData;
  return optimise(structure, optimisationData, options);
}

OptimisationOutcome
CastepGeomOptimiser::optimise(common::Structure & structure,
    OptimisationData & optimisationData,
    const OptimisationSettings & options) const
{
  const std::string outSeedName(structure.getName());

  detail::CastepGeomOptRun geomOpt(options, myCastepSeed, outSeedName,
      myCellReaderWriter, myCastepReader, mySettings);

  const OptimisationOutcome outcome = geomOpt.runFullRelax(structure,
      optimisationData, myCastepExe);
  optimisationData.saveToStructure(structure);

  return outcome;
}

void
CastepGeomOptimiser::applySettings(const Settings & settings)
{
  mySettings = settings;
}

namespace detail {

const int CastepGeomOptRun::MAX_RELAX_ATTEMPTS = 20;

namespace properties = common::structure_properties;

CastepGeomOptRun::CastepGeomOptRun(
    const OptimisationSettings & optimisationSettings,
    const std::string & originalSeed, const std::string & newSeed,
    const io::CellReaderWriter & cellReaderWriter,
    const io::CastepReader & castepReader,
    const CastepGeomOptimiseSettings & settings) :
    myCastepRun(newSeed, cellReaderWriter, castepReader), mySettings(settings), myOrigCellFile(
        originalSeed + ".cell"), myOrigParamFile(originalSeed + ".param"), myCellReaderWriter(
        cellReaderWriter), myCastepReader(castepReader), myOptimisationSettings(
        optimisationSettings)
{
}

CastepGeomOptRun::~CastepGeomOptRun()
{
  if(!mySettings.keepIntermediateFiles)
    myCastepRun.deleteAllFiles();
}

OptimisationOutcome
CastepGeomOptRun::runFullRelax(common::Structure & structure,
    OptimisationData & data, const std::string & castepExeAndArgs)
{
  if(!fs::exists(myOrigParamFile))
  {
    std::stringstream ss;
    ss << "Castep input file " << myOrigParamFile << " not found.";
    return OptimisationOutcome::failure(OptimisationError::INTERNAL_ERROR,
        ss.str());
  }

  if(!copyParamFile())
  {
    std::stringstream ss;
    ss << "Failed to copy " << myOrigParamFile << " to "
        << myCastepRun.getParamFile() << ".";
    return OptimisationOutcome::failure(OptimisationError::INTERNAL_ERROR,
        ss.str());
  }

  doPreRelaxation(structure, data, castepExeAndArgs);

  OptimisationOutcome outcome;
  int successfulRelaxations = 0;
  int i;
  for(i = 0;
      successfulRelaxations < mySettings.numConsistentRelaxations
          && i < MAX_RELAX_ATTEMPTS; ++i)
  {
    outcome = doRelaxation(structure, data, castepExeAndArgs);
    if(!outcome.isSuccess())
      return outcome;

    // Keep relaxing until we get somewhere
    if(optimisationSucceeded())
      ++successfulRelaxations;
  }
  // How did we exit?
  if(i == MAX_RELAX_ATTEMPTS)
    return OptimisationOutcome::failure(OptimisationError::FAILED_TO_CONVERGE);

  return OptimisationOutcome::success();
}

OptimisationOutcome
CastepGeomOptRun::updateStructure(common::Structure & structure,
    OptimisationData & data)
{
  const CastepRunResult::Value updateResult =
      myCastepRun.updateStructureFromOutput(structure);
  if(updateResult != CastepRunResult::SUCCESS)
  {
    if(updateResult == CastepRunResult::OUTPUT_NOT_FOUND)
    {
      std::stringstream ss;
      ss << "Castep output: " << myCastepRun.getCastepFile().string()
          << " not found.";
      return OptimisationOutcome::failure(OptimisationError::INTERNAL_ERROR,
          ss.str());
    }
    else if(updateResult == CastepRunResult::FAILED_TO_READ_STRUCTURE)
    {
      std::stringstream ss;
      ss << "Failed to read structure from "
          << myCastepRun.getCastepFile().string() << ".";
      return OptimisationOutcome::failure(OptimisationError::INTERNAL_ERROR,
          ss.str());
    }
    else
      return OptimisationOutcome::failure(OptimisationError::INTERNAL_ERROR);
  }
  parseOptimisationInfo(structure, data);
  data.saveToStructure(structure);

  return OptimisationOutcome::success();
}

bool
CastepGeomOptRun::copyParamFile() const
{
  SSLIB_ASSERT(fs::exists(myOrigParamFile));

  // Copy over the .param file to the new seed name
  try
  {
    fs::copy_file(myOrigParamFile, myCastepRun.getParamFile(),
        fs::copy_option::overwrite_if_exists);
  }
  catch(const fs::filesystem_error & /*e*/)
  {
    return false;
  }
  return true;
}

OptimisationOutcome
CastepGeomOptRun::makeCellCopy(common::Structure & structure)
{
  fs::ofstream * newCellFileStream;
  if(myCastepRun.openNewCellFile(&newCellFileStream)
      == CastepRunResult::SUCCESS)
  {
    myCellReaderWriter.writeStructure(*newCellFileStream, structure);

    if(myOptimisationSettings.pressure)
      myCellReaderWriter.writePressureBlock(*newCellFileStream,
          *myOptimisationSettings.pressure);

    // Now copy over the original contents
    if(fs::exists(myOrigCellFile))
    {
      fs::ifstream origCellStream(myOrigCellFile);
      if(origCellStream.is_open())
      {
        *newCellFileStream << "\n" << "#==ORIGINAL CONTENTS==" << "\n";
        *newCellFileStream << origCellStream.rdbuf();
        origCellStream.close();
      }
      else
      {
        std::stringstream ss;
        ss << "Failed to open " << myOrigCellFile << " for reading.";
        return OptimisationOutcome::failure(OptimisationError::INTERNAL_ERROR,
            ss.str());
      }
    }
    newCellFileStream->flush(); // Flush to get it out there
  }
  else
  {
    std::stringstream ss;
    ss << "Failed to open " << myCastepRun.getCellFile() << " for writing.";
    return OptimisationOutcome::failure(OptimisationError::INTERNAL_ERROR,
        ss.str());
  }
  return OptimisationOutcome::success();
}

OptimisationOutcome
CastepGeomOptRun::doPreRelaxation(common::Structure & structure,
    OptimisationData & optimisationData, const std::string & castepExeAndArgs)
{
  if(mySettings.numRoughSteps <= 0)
    return OptimisationOutcome::success();

  const fs::path origParamFile(myCastepRun.getParamFile().string() + ".orig");
  fs::copy_file(myCastepRun.getParamFile(), origParamFile,
      fs::copy_option::overwrite_if_exists);

  CastepRun::ParamsMap paramsMap;
  paramsMap["geom_max_iter"] = "2";
  myCastepRun.insertParams(paramsMap);

  // Do short relaxations
  for(int i = 0; i < mySettings.numRoughSteps; ++i)
    doRelaxation(structure, optimisationData, castepExeAndArgs);

  // Copy the original back
  fs::copy_file(origParamFile, myCastepRun.getParamFile(),
      fs::copy_option::overwrite_if_exists);
  fs::remove_all(origParamFile);

  return OptimisationOutcome::success();
}

OptimisationOutcome
CastepGeomOptRun::doRelaxation(common::Structure & structure,
    OptimisationData & optimistaionData, const std::string & castepExeAndArgs)
{
  // 1. Write the .cell file from the current structure
  OptimisationOutcome outcome = makeCellCopy(structure);
  if(!outcome.isSuccess())
    return outcome;

  // 2. Run Castep
  if(myCastepRun.runCastep(castepExeAndArgs) != CastepRunResult::SUCCESS)
  {
    std::stringstream ss;
    ss << "Failed to run castep with: " << castepExeAndArgs << " "
        << io::stemString(myCastepRun.getParamFile());
    return OptimisationOutcome::failure(OptimisationError::INTERNAL_ERROR,
        ss.str());
  }

  if(!myCastepRun.finishedSuccessfully())
    return OptimisationOutcome::failure(OptimisationError::INTERNAL_ERROR,
        "Castep did not finish correctly.");

  // 3. Read in results from -out.cell and update structure
  return updateStructure(structure, optimistaionData);
}

bool
CastepGeomOptRun::optimisationSucceeded()
{
  fs::ifstream * castepFileStream;
  myCastepRun.openCastepFile(&castepFileStream);

  bool succeeded = false;
  std::string line;
  if(io::findLastLine(line, *castepFileStream, "Geometry optimization"))
  {
    if(boost::find_first(line, "completed successfully"))
      succeeded = true;
  }

  return succeeded;
}

bool
CastepGeomOptRun::parseOptimisationInfo(common::Structure & structure,
    OptimisationData & data)
{
  static const std::string FORCES("* Forces *");

  fs::ifstream * castepFileStream;
  myCastepRun.openCastepFile(&castepFileStream);

  bool readSuccessfully = true;
  //std::string line;

  data.loadFromStructure(structure);

  // Forces
  //if(io::findNextLine(line, *castepFileStream, FORCES))
  //{
  //  // TODO: READ FORCES
  //}
  //else
  //  readSuccessfully = false;

  return readSuccessfully;
}

} // namespace detail

}
}

