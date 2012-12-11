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

#include <common/AtomSpeciesDatabase.h>
#include <common/Structure.h>
#include <common/UnitCell.h>
#include <io/BoostFilesystem.h>
#include <io/ResourceLocator.h>
#include <io/ResourceLocator.h>
#include <io/ResReaderWriter.h>
#include <utility/DistanceMatrixComparator.h>
#include <utility/IBufferedComparator.h>
#include <utility/StableComparison.h>
#include <utility/SortedDistanceComparator.h>
#include <utility/SortedDistanceComparatorEx.h>

namespace fs = ::boost::filesystem;
namespace ssio = ::sstbx::io;
namespace ssc = ::sstbx::common;
namespace ssu = ::sstbx::utility;

struct Result
{
  Result():
  numWrong(0),
  max(::std::numeric_limits<double>::min()),
  total(0.0)
  {}

  void calcStats(const double numComparisons)
  {
    failureRate = (double)numWrong / numComparisons;
    mean = total / numComparisons;
  }

  void printStats()
  {
    ::std::cout << "Average: " << mean << ::std::endl;
    ::std::cout << "Max: " << max << ::std::endl;
    ::std::cout << "Failure rate: " << failureRate * 100 << "%" << ::std::endl;
  }

  size_t numWrong;
  double failureRate;
  double max;
  double total;
  double mean;
};


BOOST_AUTO_TEST_CASE(StructureComparatorsTest)
{
  typedef ::boost::shared_ptr<ssc::Structure> SharedStructurePtr;
  typedef ::std::pair<fs::path, SharedStructurePtr> PathStructurePair;
  typedef ::boost::ptr_vector<ssu::IStructureComparator> Comparators;
  typedef ::boost::shared_ptr<ssu::IBufferedComparator> BufferedComparatorPtr;
  typedef ::std::vector<BufferedComparatorPtr> BufferedComparators;


  // SETTINGS ////////////////
  const fs::path referenceStructuresPath("similarStructures");
  // List of comparators to test
  Comparators comparators;
  comparators.push_back(new ssu::SortedDistanceComparator(
    ssu::SortedDistanceComparator::DEFAULT_TOLERANCE, true, true));
  //comparators.push_back(new ssu::SortedDistanceComparatorEx());
  //comparators.push_back(new ssu::DistanceMatrixComparator());
  const size_t NUM_COMPARATORS = comparators.size();
  const size_t MAX_STRUCTURES = 5;
  const double ALLOWED_FAIL_RATE = 0.005;
  const unsigned int MAX_NUM_WRONG = (unsigned int)ceil((double)MAX_STRUCTURES * ALLOWED_FAIL_RATE);

  BOOST_REQUIRE(fs::exists(referenceStructuresPath));
  BOOST_REQUIRE(fs::is_directory(referenceStructuresPath));

  ::std::vector<Result> results(NUM_COMPARATORS);

  // Use buffered comparators to make sure comparison data is not recalculated
  BufferedComparators bufferedComparators;
  bufferedComparators.reserve(NUM_COMPARATORS);
  for(size_t i = 0; i < NUM_COMPARATORS; ++i)
  {
    bufferedComparators.push_back(comparators[i].generateBuffered());
  }

  // Get the list of structures to compare

  const boost::regex resFileFilter(".*\\.res");

  std::vector< ::std::string> inputFiles;

  const fs::directory_iterator dirEnd; // Default ctor yields past-the-end
  for(fs::directory_iterator it(referenceStructuresPath); it != dirEnd; ++it )
  {
      // Skip if not a file
      if( !fs::is_regular_file(it->status())) continue;

      boost::smatch what;

      // Skip if no match
      if(!boost::regex_match(ssio::leafString(*it), what, resFileFilter)) continue;

      // File matches, store it
      inputFiles.push_back(it->path().string());
  }
  
  ssc::AtomSpeciesDatabase speciesDb;
  ssio::ResReaderWriter resReader;
  ssio::StructuresContainer structures;

  BOOST_FOREACH(::std::string & pathString, inputFiles)
  {
    fs::path strPath(pathString);
    if(!fs::exists(strPath))
    {
      ::std::cerr << "File " << strPath << " does not exist.  Skipping" << ::std::endl;
      continue;
    }

    resReader.readStructures(structures, strPath, speciesDb);

    // To make sure this test doesn't take too long limit the number of structures
    if(structures.size() >= MAX_STRUCTURES)
      break;
  }

  const size_t numStructures = structures.size();
  const double totalComparisons = 0.5 * (numStructures - 1.0) * numStructures;
  double diff;
  for(size_t i = 0; i < numStructures - 1; ++i)
  {
    for(size_t j = i + 1; j < numStructures; ++j)
    {
      for(size_t k = 0; k < NUM_COMPARATORS; ++k)
      {
        results[k].numWrong += bufferedComparators[k]->areSimilar(structures[i], structures[j]) ? 0 : 1;
        diff = bufferedComparators[k]->compareStructures(structures[i], structures[j]); 
        results[k].max = ::std::max(results[k].max, diff);
        results[k].total += diff;
      }
    }
  }

  for(size_t k = 0; k < NUM_COMPARATORS; ++k)
  {
    results[k].calcStats(totalComparisons);
    results[k].printStats();
    BOOST_REQUIRE(results[k].numWrong <= MAX_NUM_WRONG);
  }
}


BOOST_AUTO_TEST_CASE(SupercellTest)
{
  typedef ::boost::shared_ptr<ssc::Structure> SharedStructurePtr;
  typedef ::std::pair<fs::path, SharedStructurePtr> PathStructurePair;
  typedef ::boost::ptr_vector<ssu::IStructureComparator> Comparators;
  typedef ::boost::shared_ptr<ssu::IBufferedComparator> BufferedComparatorPtr;
  typedef ::std::vector<BufferedComparatorPtr> BufferedComparators;

  // SETTINGS ////////////////
  const fs::path referenceStructuresPath("similarStructures");
  // List of comparators to test
  Comparators comparators;
  comparators.push_back(new ssu::SortedDistanceComparator(
    ssu::SortedDistanceComparator::DEFAULT_TOLERANCE, false, false));
  //comparators.push_back(new ssu::SortedDistanceComparatorEx());
  //comparators.push_back(new ssu::DistanceMatrixComparator());
  const size_t NUM_COMPARATORS = comparators.size();
  const size_t MAX_STRUCTURES = 5;
  const double ALLOWED_FAIL_RATE = 0.005;
  const unsigned int MAX_NUM_WRONG = (unsigned int)ceil((double)MAX_STRUCTURES * ALLOWED_FAIL_RATE);
  const size_t SUPERCELL_DIMS[] = {1, 1, 2};

  BOOST_REQUIRE(fs::exists(referenceStructuresPath));
  BOOST_REQUIRE(fs::is_directory(referenceStructuresPath));

  ::std::vector<Result> results(NUM_COMPARATORS);

  // Use buffered comparators to make sure comparison data is not recalculated
  BufferedComparators bufferedComparators;
  bufferedComparators.reserve(NUM_COMPARATORS);
  for(size_t i = 0; i < NUM_COMPARATORS; ++i)
  {
    bufferedComparators.push_back(comparators[i].generateBuffered());
  }

  // Get the list of structures to compare
  const boost::regex resFileFilter(".*\\.res");
  std::vector<ssio::ResourceLocator> inputFiles;
  const fs::directory_iterator dirEnd; // Default ctor yields past-the-end
  for(fs::directory_iterator it(referenceStructuresPath); it != dirEnd; ++it )
  {
      // Skip if not a file
      if( !fs::is_regular_file(it->status())) continue;

      boost::smatch what;

      // Skip if no match
      if(!boost::regex_match(ssio::leafString(*it), what, resFileFilter)) continue;

      // File matches, store it
      inputFiles.push_back(it->path());
  }

  BOOST_REQUIRE(inputFiles.size() > 0);

  ssc::AtomSpeciesDatabase speciesDb;
  ssio::ResReaderWriter resReader;
  ssio::StructuresContainer structures;
  ssc::StructurePtr str;
  ssc::StructurePtr strSupercell;

  str = resReader.readStructure(inputFiles[0], speciesDb);
  //strSupercell = resReader.readStructure(inputFiles[0], speciesDb);
  strSupercell.reset(new ssc::Structure());

  ::arma::vec3 dr;
  ssc::UnitCell * cell = str->getUnitCell();
  ::arma::mat33 orthoMtx = cell->getOrthoMtx();
  for(int i = 0; i < 3; ++i)
  {
    orthoMtx.col(i) *= SUPERCELL_DIMS[i];
  }
  strSupercell->setUnitCell(ssc::UnitCellPtr(new ssc::UnitCell(orthoMtx)));
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
          strSupercell->newAtom(origAtom.getSpecies()).setPosition(origAtom.getPosition() + dr);
        }

      }
    }
  }
  resReader.writeStructure(*strSupercell, ssio::ResourceLocator("supercell.res"), speciesDb);

  for(size_t i = 0; i < NUM_COMPARATORS; ++i)
  {
    double diff = bufferedComparators[i]->compareStructures(*str.get(), *strSupercell.get());
    BOOST_REQUIRE(ssu::StableComp::eq(diff, 1e-9));
  }
  
}
