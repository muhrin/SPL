/*
 * ExternalOptimiserTest.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "sslibtest.h"

#include <armadillo>

#include <spl/common/Structure.h>
#include <spl/common/UnitCell.h>
#include <spl/potential/ExternalOptimiser.h>
#include <spl/potential/OptimisationSettings.h>

// NAMESPACES ///////////////////////////////
using namespace spl;

BOOST_AUTO_TEST_CASE(ExternalOptimiserTest)
{
  potential::ExternalOptimiser optimiser("external_optimiser.sh");

  common::Structure structure;
  structure.setName("NaCl-external_optimise");
  structure.setUnitCell(common::UnitCell()); // Create a 1 1 1 90 90 90 unit cell
  structure.newAtom("Na").setPosition(structure.getUnitCell()->randomPoint());
  structure.newAtom("Na").setPosition(structure.getUnitCell()->randomPoint());
  structure.newAtom("Cl").setPosition(structure.getUnitCell()->randomPoint());
  structure.newAtom("Cl").setPosition(structure.getUnitCell()->randomPoint());

  potential::OptimisationSettings optSettings;
  optSettings.energyTol = 2e-5;
  optSettings.forceTol = 5e-2;
  optSettings.stressTol = 1e-5;
  optSettings.pressure = arma::eye< arma::mat>(3, 3);
  optSettings.optimisationType =
      potential::OptimisationSettings::Optimise::ATOMS_AND_LATTICE;

  potential::OptimisationData optData;
  BOOST_REQUIRE(optimiser.optimise(structure, optData, optSettings));

  // Check that we got the output we expected
  BOOST_REQUIRE(optData.enthalpy);
  BOOST_CHECK_EQUAL(*optData.enthalpy, 2.0);
  BOOST_REQUIRE(optData.internalEnergy);
  BOOST_CHECK_EQUAL(*optData.internalEnergy, 1.0);
  BOOST_REQUIRE(optData.pressure);
  BOOST_CHECK_EQUAL(*optData.pressure, 0.0);
  BOOST_REQUIRE(optData.numIters);
  BOOST_CHECK_EQUAL(*optData.numIters, 1000);
}
