/*
 * StructureBuilderTest.cpp
 *
 *  Created on: Oct 3, 2012
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "sslibtest.h"

#include <boost/foreach.hpp>

#include <spl/build_cell/BuildCellFwd.h>
#include <spl/build_cell/AtomsDescription.h>
#include <spl/build_cell/AtomsGroup.h>
#include <spl/build_cell/IUnitCellGenerator.h>
#include <spl/build_cell/GenerationOutcome.h>
#include <spl/build_cell/StructureBuilder.h>
#include <spl/common/AtomSpeciesDatabase.h>
#include <spl/common/AtomSpeciesId.h>
#include <spl/common/Structure.h>
#include <spl/common/Types.h>

namespace ssbc = ::spl::build_cell;
namespace ssc  = ::spl::common;


BOOST_AUTO_TEST_CASE(StructureBuilderClusterTest)
{
  typedef ::std::pair<ssc::AtomSpeciesId::Value, unsigned int> SpeciesCount;
  //// Settings ////////////////
  const unsigned int TIMES_TO_GENERATE = 10;
  
  ::std::vector<SpeciesCount> toGenerate;
  toGenerate.push_back(SpeciesCount("Na", 2));
  toGenerate.push_back(SpeciesCount("Cl", 5));
  toGenerate.push_back(SpeciesCount("H", 10));

  unsigned int totalAtoms = 0;
  BOOST_FOREACH(const SpeciesCount & speciesCount, toGenerate)
    totalAtoms += speciesCount.second;
  
  ssbc::StructureBuilder builder;
  {
    ::spl::UniquePtr<ssbc::AtomsGroup>::Type atomsGenerator(new ssbc::AtomsGroup());

    BOOST_FOREACH(const SpeciesCount & speciesCount, toGenerate)
      atomsGenerator->insertAtoms(ssbc::AtomsDescription(speciesCount.first, speciesCount.second));
    
    builder.addGenerator(atomsGenerator);
  }

  ssc::AtomSpeciesDatabase speciesDb;
  ssc::StructurePtr structure;
  ssbc::GenerationOutcome outcome;
  for(unsigned int i = 0; i < TIMES_TO_GENERATE; ++i)
  {
    outcome = builder.generateStructure(structure, speciesDb);
    BOOST_CHECK(outcome.isSuccess());
    BOOST_REQUIRE(structure->getNumAtoms() == totalAtoms);
  }


}
