/*
 * AtomExtruderTest.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "sslibtest.h"

#include <vector>
#include <iostream>

#include <armadillo>

#include <build_cell/AtomExtruder.h>
#include <build_cell/RandomUnitCell.h>
#include <common/Atom.h>
#include <common/AtomSpeciesId.h>
#include <common/Constants.h>
#include <common/DistanceCalculator.h>
#include <common/Structure.h>
#include <common/UnitCell.h>
#include <common/Types.h>

#include <common/Utils.h>

namespace ssbc = ::sstbx::build_cell;
namespace ssc = ::sstbx::common;

BOOST_AUTO_TEST_CASE(ExtrusionTest)
{
  // SETTINGS ///////
  const size_t numStructures = 5, maxAtoms = 10;

  ssbc::RandomUnitCell randomCell;
  randomCell.setVolumeDelta(0.0);

  ssbc::AtomExtruder extruder;

  const double radius = 1.0, minsep = 2.0 * radius - 0.1, minsepSq = minsep * minsep;
  size_t numAtoms;

  bool extruded;
  double volume;
  for(size_t i = 0; i < numStructures; ++i)
  {
    extruded = false;

    ssc::Structure structure;

    numAtoms = (size_t)ssc::randDouble(1.0, (double)maxAtoms);

    // Create a unit cell
    // Make the volume somewhat bigger than the space filled by the atoms
    randomCell.setTargetVolume(2.0 * numAtoms * 4.0 / 3.0 * ssc::Constants::PI /* times r^3, but r=1 */);

    structure.setUnitCell(randomCell.generateCell());
    const ssc::UnitCell * const cell = structure.getUnitCell();

    // Check that the volume is not NaN
    volume = cell->getVolume();
    BOOST_CHECK(volume == volume);

    for(size_t j = 0; j < numAtoms; ++j)
    {
      ssc::Atom & atom = structure.newAtom(ssc::AtomSpeciesId::CUSTOM_1);
      atom.setPosition(cell->randomPoint());
    }

    extruded = extruder.extrudeAtoms(structure);

    if(extruded)
    {
      // Check that they are indeed no closer than 2 apart
      const ssc::DistanceCalculator & distanceCalc = structure.getDistanceCalculator();
      double dr;

      for(size_t k = 0; k < numAtoms - 1; ++k)
      {
        const ::arma::vec & pos1 = structure.getAtom(k).getPosition();
        for(size_t l = k + 1; l < numAtoms; ++l)
        {
          const ::arma::vec & pos2 = structure.getAtom(l).getPosition();
          dr = distanceCalc.getDistMinImg(pos1, pos2);

          BOOST_REQUIRE(dr * dr >= minsepSq);
        }
      }
    }
    else
    {
      BOOST_WARN_MESSAGE(extruded, "Extruder failed to extrude atoms");
    }
  }
}
