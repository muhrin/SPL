/*
 * AtomExtruderTest.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "sslibtest.h"

#include <iterator>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <spl/common/AtomSpeciesId.h>
#include <spl/common/Structure.h>
#include <spl/common/Types.h>
#include <spl/common/UnitCell.h>
#include <spl/io/CellReaderWriter.h>
#include <spl/io/InfoLine.h>
#include <spl/io/ResourceLocator.h>
#include <spl/io/ResReaderWriter.h>
#include <spl/io/SplReaderWriter.h>
#include <spl/io/StructureReadWriteManager.h>
#include <spl/io/XyzReaderWriter.h>
#include <spl/math/Random.h>
#include <spl/utility/StableComparison.h>

BOOST_AUTO_TEST_SUITE(ReaderWriter)

namespace fs = boost::filesystem;
namespace ssio = spl::io;
namespace ssc = spl::common;
namespace properties = ssc::structure_properties;

void
checkSimilar(const ssc::Structure & str1, const ssc::Structure & str2);

BOOST_AUTO_TEST_CASE(SimilarityTest)
{
  // SETTINGS ///////
  const size_t NUM_ATOMS = 32;
  const std::string SAVE_PATH = "rwTest.out";

  // Set up all the readers/writers we want to test
  ssio::StructureReadWriteManager rwMan;
  rwMan.insert(spl::makeUniquePtr(new ssio::ResReaderWriter()));
  rwMan.insert(spl::makeUniquePtr(new ssio::SplReaderWriter()));
  rwMan.insert(spl::makeUniquePtr(new ssio::CellReaderWriter()));
  rwMan.insert(spl::makeUniquePtr(new ssio::XyzReaderWriter()));

  // Set up the structure to write
  ssc::Structure structure;
  ssc::AtomSpeciesId::Value species = "Na";
  for(size_t i = 0; i < NUM_ATOMS; ++i)
    structure.newAtom(species).setPosition(arma::randu(3));

  ssio::ResourceLocator saveTo;
  saveTo.setId("rwTest");
  ssc::StructurePtr loadedStructure;
  for(ssio::StructureReadWriteManager::WritersIterator it =
      rwMan.beginWriters(), end = rwMan.endWriters(); it != end; ++it)
  {
    saveTo.setPath(fs::path(SAVE_PATH + "." + it->first));
    it->second->writeStructure(structure, saveTo);

    loadedStructure = rwMan.readStructure(saveTo);
    BOOST_CHECK(loadedStructure.get());

    if(loadedStructure.get())
      checkSimilar(structure, *loadedStructure);
  }

}

BOOST_AUTO_TEST_CASE(InfoLineStream)
{
  // SETTINGS //
  static const std::string LINE =
      "voronoi-4626-rQvm6-6 -0.01455109 4622.6610614649080 -334.713226852853 0 0 194 (P1) n - 1";
  static const double DOUBLE_CLOSENESS = 0.000001;

  std::stringstream ss(LINE);
  ssio::InfoLine infoLine;
  ss >> infoLine;

  BOOST_REQUIRE(infoLine.name.is_initialized());
  BOOST_CHECK_EQUAL(*infoLine.name, "voronoi-4626-rQvm6-6");

  BOOST_REQUIRE(infoLine.pressure.is_initialized());
  BOOST_CHECK_CLOSE_FRACTION(*infoLine.pressure, -0.01455109, DOUBLE_CLOSENESS);

  BOOST_REQUIRE(infoLine.volume.is_initialized());
  BOOST_CHECK_CLOSE_FRACTION(*infoLine.volume, 4622.6610614649080, DOUBLE_CLOSENESS);

  BOOST_REQUIRE(infoLine.enthalpy.is_initialized());
  BOOST_CHECK_CLOSE_FRACTION(*infoLine.enthalpy, -334.713226852853, DOUBLE_CLOSENESS);

  BOOST_REQUIRE(infoLine.numAtoms.is_initialized());
  BOOST_CHECK_EQUAL(*infoLine.numAtoms, 194);

  BOOST_REQUIRE(infoLine.spaceGroup.is_initialized());
  BOOST_CHECK_EQUAL(*infoLine.spaceGroup, "P1");

  BOOST_REQUIRE(infoLine.timesFound.is_initialized());
  BOOST_CHECK_EQUAL(*infoLine.timesFound, 1);
}

BOOST_AUTO_TEST_CASE(InfoLineStructure)
{
  // SETTINGS //
  static const std::string NAME = "test structure";
  static const double PRESSURE = spl::math::randu< double>();
  static const double ENTHALPY = spl::math::randu< double>();
  static const size_t MAX_ATOMS = 10;
  static const std::string SPACE_GROUP = "Fm-3m";
  static const unsigned int TIMES_FOUND = spl::math::randu< unsigned int>(100);

  ssc::Structure structure;
  ssio::InfoLine infoLine(structure);

  BOOST_CHECK(!infoLine.name.is_initialized());
  BOOST_CHECK(!infoLine.pressure.is_initialized());
  BOOST_CHECK(!infoLine.volume.is_initialized());
  BOOST_CHECK(!infoLine.enthalpy.is_initialized());
  BOOST_CHECK(!infoLine.spinDensity.is_initialized());
  BOOST_CHECK(!infoLine.integratedSpinDensity.is_initialized());
  BOOST_REQUIRE(infoLine.numAtoms.is_initialized());
  BOOST_CHECK_EQUAL(*infoLine.numAtoms, structure.getNumAtoms());
  BOOST_CHECK(!infoLine.spaceGroup.is_initialized());
  BOOST_CHECK(!infoLine.timesFound.is_initialized());

  // Name
  structure.setName("test structure");
  infoLine.set(structure);
  BOOST_REQUIRE(infoLine.name.is_initialized());
  BOOST_CHECK_EQUAL(*infoLine.name, NAME);

  // Pressure
  structure.setProperty(properties::general::PRESSURE, PRESSURE);
  infoLine.set(structure);
  BOOST_REQUIRE(infoLine.pressure.is_initialized());
  BOOST_CHECK_EQUAL(*infoLine.pressure, PRESSURE);

  // Volume
  structure.setUnitCell(ssc::UnitCell());
  infoLine.set(structure);
  BOOST_REQUIRE(infoLine.volume.is_initialized());
  BOOST_CHECK_EQUAL(*infoLine.volume, structure.getUnitCell()->getVolume());

  // ENTHALPY
  structure.setProperty(properties::general::ENTHALPY, ENTHALPY);
  infoLine.set(structure);
  BOOST_REQUIRE(infoLine.enthalpy.is_initialized());
  BOOST_CHECK_EQUAL(*infoLine.enthalpy, ENTHALPY);

  // NUM ATOMS
  for(size_t i = 0; i < spl::math::randu(MAX_ATOMS); ++i)
    structure.newAtom("Na");
  infoLine.set(structure);
  BOOST_REQUIRE(infoLine.numAtoms.is_initialized());
  BOOST_CHECK_EQUAL(*infoLine.numAtoms, structure.getNumAtoms());

  // SPACE GROUP
  structure.setProperty(properties::general::SPACEGROUP_SYMBOL, SPACE_GROUP);
  infoLine.set(structure);
  BOOST_REQUIRE(infoLine.spaceGroup.is_initialized());
  BOOST_CHECK_EQUAL(*infoLine.spaceGroup, SPACE_GROUP);

  // TIMES FOUND
  structure.setProperty(properties::searching::TIMES_FOUND, TIMES_FOUND);
  infoLine.set(structure);
  BOOST_REQUIRE(infoLine.timesFound.is_initialized());
  BOOST_CHECK_EQUAL(*infoLine.timesFound, TIMES_FOUND);

  // Now check that the line populates structure properties correctly
  ssc::Structure newStructure;
  infoLine.populate(&newStructure);
  BOOST_CHECK_EQUAL(*infoLine.name, newStructure.getName());

  BOOST_REQUIRE(newStructure.getProperty(properties::general::PRESSURE));
  BOOST_CHECK_EQUAL(*infoLine.pressure,
      *newStructure.getProperty(properties::general::PRESSURE));

  BOOST_REQUIRE(newStructure.getProperty(properties::general::ENTHALPY));
  BOOST_CHECK_EQUAL(*infoLine.enthalpy,
      *newStructure.getProperty(properties::general::ENTHALPY));

  BOOST_REQUIRE(
      newStructure.getProperty(properties::general::SPACEGROUP_SYMBOL));
  BOOST_CHECK_EQUAL(*infoLine.spaceGroup,
      *newStructure.getProperty(properties::general::SPACEGROUP_SYMBOL));

  BOOST_REQUIRE(newStructure.getProperty(properties::searching::TIMES_FOUND));
  BOOST_CHECK_EQUAL(*infoLine.timesFound,
      *newStructure.getProperty(properties::searching::TIMES_FOUND));
}

void
checkSimilar(const ssc::Structure & str1, const ssc::Structure & str2)
{
  BOOST_CHECK_EQUAL(str1.getNumAtoms(), str2.getNumAtoms());

  std::vector< ssc::AtomSpeciesId::Value> species1, species2;
  str1.getAtomSpecies(std::back_inserter(species1));
  str2.getAtomSpecies(std::back_inserter(species2));

  BOOST_CHECK_EQUAL(species1.size(), species2.size());

  // Put all the species in structure 1 into a set and one by one insert those from
  // structure 2 and make sure no new ones are inserted
  std::set< ssc::AtomSpeciesId::Value> speciesSet(species1.begin(),
      species1.end());
  BOOST_FOREACH(const ssc::AtomSpeciesId::Value & spec, species2)
  {
    BOOST_REQUIRE(!speciesSet.insert(spec).second);
    // Check the numbers of species match
    BOOST_CHECK_EQUAL(str1.getNumAtomsOfSpecies(spec),
        str2.getNumAtomsOfSpecies(spec));
  }

  // Check the unit cell (if any)
  //const ssc::UnitCell * const uc1 = str1.getUnitCell();
  //const ssc::UnitCell * const uc2 = str2.getUnitCell();

  //BOOST_REQUIRE(!((uc1 == NULL) ^ (uc2 == NULL))); // Either have to be both NULL or both !NULL
  //if(uc1 && uc2)
  //{
  //  BOOST_REQUIRE(compare::eq(uc1->getVolume(), uc2->getVolume()));

  //  // TODO: More unit cell checks

  //}
}

BOOST_AUTO_TEST_SUITE_END()
