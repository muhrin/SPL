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

#include <spl/build_cell/GenerationOutcome.h>
#include <spl/build_cell/PointSeparator.h>
#include <spl/build_cell/RandomUnitCellGenerator.h>
#include <spl/common/Atom.h>
#include <spl/common/AtomSpeciesId.h>
#include <spl/common/Constants.h>
#include <spl/common/DistanceCalculator.h>
#include <spl/common/Structure.h>
#include <spl/common/UnitCell.h>
#include <spl/common/Types.h>
#include <spl/math/Random.h>
#include <spl/utility/StableComparison.h>

namespace ssbc = spl::build_cell;
namespace ssc = spl::common;
namespace ssm = spl::math;

BOOST_AUTO_TEST_SUITE(PointSeparator)

BOOST_AUTO_TEST_CASE(ExtrusionTest)
{
  // SETTINGS ///////
  const size_t numStructures = 5, maxAtoms = 10;
  const double SEPARATION_TOL = 0.001;

  ssbc::RandomUnitCellGenerator randomCell;
  randomCell.setVolumeDelta(0.0);

  ssbc::PointSeparator separator(ssbc::PointSeparator::DEFAULT_MAX_ITERATIONS,
      SEPARATION_TOL);

  const double radius = 1.0, minsep = 2.0 * radius;
  size_t numAtoms;

  bool extruded;
  double volume;
  for(size_t i = 0; i < numStructures; ++i)
  {
    extruded = false;

    ssc::Structure structure;

    numAtoms = static_cast< size_t>(ssm::randu(1,
        static_cast< int>(maxAtoms) - 1));

    // Create a unit cell
    // Make the volume somewhat bigger than the space filled by the atoms
    randomCell.setTargetVolume(
        2.0 * numAtoms * 4.0 / 3.0
            * ssc::constants::PI /* times r^3, but r=1 */);

    {
      ssc::UnitCellPtr cell;
      BOOST_REQUIRE(randomCell.generateCell(cell).isSuccess());
      structure.setUnitCell(*cell);
    }
    const ssc::UnitCell * const cell = structure.getUnitCell();
    BOOST_REQUIRE(cell);

    // Check that the volume is not NaN
    volume = cell->getVolume();
    BOOST_CHECK(volume == volume);

    for(size_t j = 0; j < numAtoms; ++j)
    {
      ssc::Atom & atom = structure.newAtom("C1");
      atom.setPosition(cell->randomPoint());
    }

    ssbc::SeparationData sepData(structure);
    sepData.separations.fill(minsep);
    extruded = separator.separatePoints(&sepData);

    if(extruded)
    {
      structure.setAtomPositions(sepData.points);

      // Check that they are indeed no closer than the minsep apart
      const ssc::DistanceCalculator & distanceCalc =
          structure.getDistanceCalculator();
      double drSq;

      for(size_t k = 0; k < numAtoms - 1; ++k)
      {
        const arma::vec & pos1 = structure.getAtom(k).getPosition();
        for(size_t l = k + 1; l < numAtoms; ++l)
        {
          const arma::vec & pos2 = structure.getAtom(l).getPosition();
          drSq = distanceCalc.getDistSqMinImg(pos1, pos2);

          BOOST_CHECK_GE(std::sqrt(drSq), (1.0 - SEPARATION_TOL) * minsep);
        }
      }
    }
    else
      BOOST_WARN_MESSAGE(extruded, "Separator failed to extrude atoms");
  }
}

BOOST_AUTO_TEST_SUITE_END()
