/*
 * DistanceCalculatorsTest.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "sslibtest.h"

#include <iostream>
#include <limits>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>

#include <spl/common/Structure.h>
#include <spl/common/UnitCell.h>
#include <spl/io/BoostFilesystem.h>
#include <spl/io/ResourceLocator.h>
#include <spl/io/ResourceLocator.h>
#include <spl/io/ResReaderWriter.h>
#include <spl/utility/DistanceMatrixComparator.h>
#include <spl/utility/IBufferedComparator.h>
#include <spl/utility/StableComparison.h>
#include <spl/utility/SortedDistanceComparator.h>
#include <spl/utility/SortedDistanceComparatorEx.h>

namespace fs = boost::filesystem;
namespace ssio = spl::io;
namespace ssc = spl::common;
namespace ssu = spl::utility;

struct Result
{
  Result() :
      numWrong(0), failureRate(0.0), max(std::numeric_limits< double>::min()), total(
          0.0), mean(0.0)
  {
  }

  void
  calcStats(const double numComparisons)
  {
    failureRate = static_cast< double>(numWrong) / numComparisons;
    mean = total / numComparisons;
  }

  void
  printStats()
  {
    std::cout << "Average: " << mean << std::endl;
    std::cout << "Max: " << max << std::endl;
    std::cout << "Failure rate: " << failureRate * 100.0 << "%" << std::endl;
  }

  size_t numWrong;
  double failureRate;
  double max;
  double total;
  double mean;
};

static const fs::path REFERENCE_STRUCTURES_PATH("similarStructures");

BOOST_AUTO_TEST_SUITE(StructureComparators)

BOOST_AUTO_TEST_CASE(StructureComparatorsTest)
{
  typedef boost::ptr_vector< ssu::IStructureComparator> Comparators;
  typedef boost::shared_ptr< ssu::IBufferedComparator> BufferedComparatorPtr;
  typedef std::vector< BufferedComparatorPtr> BufferedComparators;
  typedef ssu::IBufferedComparator::ComparisonDataHandle ComparisonDataHandle;
  typedef std::vector< ComparisonDataHandle> ComparisonHandles;

  // SETTINGS ////////////////
  // List of comparators to test
  Comparators comparators;
  {
    ssu::SortedDistanceComparator::ConstructionInfo constructInfo;
    constructInfo.volumeAgnostic = true;
    constructInfo.usePrimitive = true;
    comparators.push_back(new ssu::SortedDistanceComparator(constructInfo));
  }
  //comparators.push_back(new ssu::SortedDistanceComparatorEx());
  comparators.push_back(new ssu::DistanceMatrixComparator());
  const size_t NUM_COMPARATORS = comparators.size();
  const size_t MAX_STRUCTURES = 100;
  const double ALLOWED_FAIL_RATE = 0.02;

  BOOST_REQUIRE(fs::exists(REFERENCE_STRUCTURES_PATH));
  BOOST_REQUIRE(fs::is_directory(REFERENCE_STRUCTURES_PATH));

  std::vector< Result> results(NUM_COMPARATORS);
  ComparisonHandles comparisonHandles;

  // Use buffered comparators to make sure comparison data is not recalculated
  BufferedComparators bufferedComparators;
  bufferedComparators.reserve(NUM_COMPARATORS);
  for(size_t i = 0; i < NUM_COMPARATORS; ++i)
  {
    bufferedComparators.push_back(comparators[i].generateBuffered());
  }

  // Get the list of structures to compare

  const boost::regex resFileFilter(".*\\.res");

  std::vector< std::string> inputFiles;

  const fs::directory_iterator dirEnd; // Default ctor yields past-the-end
  for(fs::directory_iterator it(REFERENCE_STRUCTURES_PATH); it != dirEnd; ++it)
  {
    // Skip if not a file
    if(!fs::is_regular_file(it->status()))
      continue;

    boost::smatch what;

    // Skip if no match
    if(!boost::regex_match(ssio::leafString(*it), what, resFileFilter))
      continue;

    // File matches, store it
    inputFiles.push_back(it->path().string());
  }

  ssio::ResReaderWriter resReader;
  ssio::StructuresContainer structures;

  BOOST_FOREACH(std::string & pathString, inputFiles)
  {
    fs::path strPath(pathString);
    if(!fs::exists(strPath))
    {
      std::cerr << "File " << strPath << " does not exist.  Skipping"
          << std::endl;
      continue;
    }

    resReader.readStructures(structures, strPath);

    // To make sure this test doesn't take too long limit the number of structures
    if(structures.size() >= MAX_STRUCTURES)
      break;
  }

  const size_t numStructures = structures.size();

  comparisonHandles.resize(numStructures);
  const double totalComparisons = 0.5 * (numStructures - 1.0) * numStructures;
  const size_t maxWrong = static_cast< size_t>(std::ceil(
      totalComparisons * ALLOWED_FAIL_RATE));
  double diff;
  for(size_t k = 0; k < NUM_COMPARATORS; ++k)
  {
    for(size_t i = 0; i < numStructures; ++i)
    {
      comparisonHandles[i] = bufferedComparators[k]->generateComparisonData(
          structures[i]);
    }
    for(size_t i = 0; i < numStructures - 1; ++i)
    {
      for(size_t j = i + 1; j < numStructures; ++j)
      {
        //ssc::StructurePtr primitive = structures[i].second->getPrimitiveCopy();
        //resReader.writeStructure(*primitive.get(), "primitive.res", speciesDb);
        results[k].numWrong +=
            bufferedComparators[k]->areSimilar(comparisonHandles[i],
                comparisonHandles[j]) ? 0 : 1;
        diff = bufferedComparators[k]->compareStructures(comparisonHandles[i],
            comparisonHandles[j]);
        results[k].max = std::max(results[k].max, diff);
        results[k].total += diff;
      }
    }
    comparisonHandles.clear();
  }

  for(size_t k = 0; k < NUM_COMPARATORS; ++k)
  {
    results[k].calcStats(totalComparisons);
    results[k].printStats();
    BOOST_REQUIRE(results[k].numWrong <= maxWrong);
  }
}

BOOST_AUTO_TEST_CASE(SupercellTest)
{
  typedef boost::ptr_vector< ssu::IStructureComparator> Comparators;
  typedef boost::shared_ptr< ssu::IBufferedComparator> BufferedComparatorPtr;
  typedef std::vector< BufferedComparatorPtr> BufferedComparators;

  // SETTINGS ////////////////
  // List of comparators to test
  Comparators comparators;
  {
    ssu::SortedDistanceComparator::ConstructionInfo constructInfo;
    constructInfo.volumeAgnostic = false;
    constructInfo.usePrimitive = false;
    comparators.push_back(new ssu::SortedDistanceComparator(constructInfo));
  }
  //comparators.push_back(new ssu::SortedDistanceComparatorEx());
  comparators.push_back(new ssu::DistanceMatrixComparator());
  const size_t NUM_COMPARATORS = comparators.size();
  const size_t SUPERCELL_DIMS[] =
    { 1, 1, 2 };

  BOOST_REQUIRE(fs::exists(REFERENCE_STRUCTURES_PATH));
  BOOST_REQUIRE(fs::is_directory(REFERENCE_STRUCTURES_PATH));

  std::vector< Result> results(NUM_COMPARATORS);

  // Use buffered comparators to make sure comparison data is not recalculated
  BufferedComparators bufferedComparators;
  bufferedComparators.reserve(NUM_COMPARATORS);
  for(size_t i = 0; i < NUM_COMPARATORS; ++i)
  {
    bufferedComparators.push_back(comparators[i].generateBuffered());
  }

  // Get the list of structures to compare
  const boost::regex resFileFilter(".*\\.res");
  std::vector< ssio::ResourceLocator> inputFiles;
  const fs::directory_iterator dirEnd; // Default ctor yields past-the-end
  for(fs::directory_iterator it(REFERENCE_STRUCTURES_PATH); it != dirEnd; ++it)
  {
    // Skip if not a file
    if(!fs::is_regular_file(it->status()))
      continue;

    boost::smatch what;

    // Skip if no match
    if(!boost::regex_match(ssio::leafString(*it), what, resFileFilter))
      continue;

    // File matches, store it
    inputFiles.push_back(it->path());
  }

  BOOST_REQUIRE(inputFiles.size() > 0);

  ssio::ResReaderWriter resReader;
  ssio::StructuresContainer structures;
  ssc::StructurePtr str;
  ssc::StructurePtr strSupercell;

  str = resReader.readStructure(inputFiles[0]);
  //strSupercell = resReader.readStructure(inputFiles[0], speciesDb);
  strSupercell.reset(new ssc::Structure());

  arma::vec3 dr;
  ssc::UnitCell * cell = str->getUnitCell();
  arma::mat33 orthoMtx = cell->getOrthoMtx();
  for(int i = 0; i < 3; ++i)
  {
    orthoMtx.col(i) *= SUPERCELL_DIMS[i];
  }
  strSupercell->setUnitCell(ssc::UnitCell(orthoMtx));
  for(size_t i = 0; i < SUPERCELL_DIMS[0]; ++i)
  {
    for(size_t j = 0; j < SUPERCELL_DIMS[1]; ++j)
    {
      for(size_t k = 0; k < SUPERCELL_DIMS[2]; ++k)
      {
        dr = cell->getAVec() * i + cell->getBVec() * j + cell->getCVec() * k;

        for(size_t atom = 0; atom < str->getNumAtoms(); ++atom)
        {
          const ssc::Atom & origAtom = str->getAtom(atom);
          strSupercell->newAtom(origAtom.getSpecies()).setPosition(
              origAtom.getPosition() + dr);
        }

      }
    }
  }
  resReader.writeStructure(*strSupercell,
      ssio::ResourceLocator("supercell.res"));

  for(size_t i = 0; i < NUM_COMPARATORS; ++i)
  {
    double diff = comparators[i].compareStructures(*str.get(),
        *strSupercell.get());
    BOOST_REQUIRE(ssu::stable::eq(diff, 1e-9));
  }

}

BOOST_AUTO_TEST_SUITE_END()
