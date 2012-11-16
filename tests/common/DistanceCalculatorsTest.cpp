/*
 * DistanceCalculatorsTest.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "sslibtest.h"

#include <iostream>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <armadillo>

#include <build_cell/RandomUnitCell.h>
#include <common/Atom.h>
#include <common/OrthoCellDistanceCalculator.h>
#include <common/ReferenceDistanceCalculator.h>
#include <common/Structure.h>
#include <common/Types.h>
#include <common/UniversalCrystalDistanceCalculator.h>
#include <common/UnitCell.h>
#include <common/Utils.h>
#include <utility/StableComparison.h>

//#define DIST_TEST_MANUAL_DEBUG

namespace ssbc = ::sstbx::build_cell;
namespace ssc = ::sstbx::common;
namespace ssu = ::sstbx::utility;


BOOST_AUTO_TEST_CASE(OrthogonalUnitCellComparison)
{
  // SETTINGS ////////////////
  const double cellDim = 1;
  const double tolerance = 1e-13;
#ifdef DIST_TEST_MANUAL_DEBUG
  const size_t numAtoms = 15;
  const double maxCutoff = 2 * cellDim;
#else
  const size_t numAtoms = 50;
  const double maxCutoff = 4 * cellDim;
#endif
  const size_t numAttempts = 100;

  // Timers
  time_t t0, t1, tOrtho = 0, tUniv = 0;
  double cutoff;
  ::std::vector<double> referenceDists, orthoDist, univDist;
  ::std::vector< ::arma::vec3> referenceVecs, orthoVecs, univVecs;
  ::arma::vec3 orthoSum, univSum, referenceSum;
  for(size_t attempt = 0; attempt < numAttempts; ++attempt)
  {
    referenceDists.clear(); referenceVecs.clear();
    orthoDist.clear(); orthoVecs.clear();
    univDist.clear(); univVecs.clear();

    ssc::Structure structure;
    structure.setUnitCell(ssc::UnitCellPtr(new ssc::UnitCell(
      ssc::randDouble(0.1, 5) * cellDim,
      ssc::randDouble(0.1, 5) * cellDim, 
      ssc::randDouble(0.1, 5) * cellDim, 90.0, 90.0, 90.0)));

    for(size_t i = 0; i < numAtoms; ++i)
      structure.newAtom(ssc::AtomSpeciesId::CUSTOM_1).setPosition(structure.getUnitCell()->randomPoint());

    ssc::OrthoCellDistanceCalculator orthoCalc(structure);
    ssc::UniversalCrystalDistanceCalculator univCalc(structure);
    ssc::ReferenceDistanceCalculator referenceCalc(structure);

    BOOST_REQUIRE(orthoCalc.isValid());
    BOOST_REQUIRE(univCalc.isValid());
    BOOST_REQUIRE(referenceCalc.isValid());

    cutoff = ssc::randDouble(0.0, maxCutoff);

    t0 = time(NULL);

    for(size_t i = 0; i < numAtoms; ++i)
    {
      const ssc::Atom atom1 = structure.getAtom(i);
      for(size_t j = i; j < numAtoms; ++j)
      {
        const ssc::Atom atom2 = structure.getAtom(j);
        univCalc.getDistsBetween(atom1, atom2, cutoff, univDist);
        univCalc.getVecsBetween(atom1, atom2, cutoff, univVecs);
      }
    }

    t1 = time(NULL);

    tUniv += t1 - t0;

    t0 = time(NULL);

    for(size_t i = 0; i < numAtoms; ++i)
    {
      const ssc::Atom atom1 = structure.getAtom(i);
      for(size_t j = i; j < numAtoms; ++j)
      {
        const ssc::Atom atom2 = structure.getAtom(j);
        orthoCalc.getDistsBetween(atom1, atom2, cutoff, orthoDist);
        orthoCalc.getVecsBetween(atom1, atom2, cutoff, orthoVecs);
      }
    }

    t1 = time(NULL);

    tOrtho += t1 - t0;

    for(size_t i = 0; i < numAtoms; ++i)
    {
      const ssc::Atom atom1 = structure.getAtom(i);
      for(size_t j = i; j < numAtoms; ++j)
      {
        const ssc::Atom atom2 = structure.getAtom(j);
        referenceCalc.getDistsBetween(atom1, atom2, cutoff, referenceDists);
        referenceCalc.getVecsBetween(atom1, atom2, cutoff, referenceVecs);
      }
    }

#ifdef DIST_TEST_MANUAL_DEBUG
    if(orthoVecs.size() != univVecs.size() || orthoVecs.size() != univVecs.size())
      ::std::cout << "Vector size mismatch.\n";
#endif

    BOOST_REQUIRE(orthoVecs.size() == referenceVecs.size());
    BOOST_REQUIRE(univVecs.size() == referenceVecs.size());

    ::std::sort(orthoDist.begin(), orthoDist.end());
    ::std::sort(univDist.begin(), univDist.end());
    ::std::sort(referenceDists.begin(), referenceDists.end());

    const size_t numElements = ::std::min(orthoDist.size(), univDist.size());

    referenceSum.zeros();
    univSum.zeros();
    orthoSum.zeros();
    for(size_t i = 0; i < numElements; ++i)
    {
      referenceSum += referenceVecs[i];
      orthoSum += orthoVecs[i];
      univSum += univVecs[i];

#ifdef DIST_TEST_MANUAL_DEBUG
      if(!ssu::StableComp::eq(orthoDist[i], referenceDists[i], tolerance))
        ::std::cout << "Diff. in distance (ortho - reference): " << orthoDist[i] - referenceDists[i] << ::std::endl;
      if(!ssu::StableComp::eq(univDist[i], referenceDists[i], tolerance))
        ::std::cout << "Diff. in distance (universal - reference)" << univDist[i] - referenceDists[i] << ::std::endl;
#endif

      BOOST_REQUIRE(ssu::StableComp::eq(orthoDist[i], referenceDists[i], tolerance));
      BOOST_REQUIRE(ssu::StableComp::eq(univDist[i], referenceDists[i], tolerance));
    }
    // Check that the components of the sum of the vectors match
    for(size_t i = 0; i < 3; ++i)
    {

#ifdef DIST_TEST_MANUAL_DEBUG
      if(!ssu::StableComp::eq(orthoSum(i), referenceSum(i), 3e-9))
        ::std::cout << "Diff: " << orthoSum(i) - referenceSum(i) << ::std::endl;
      if(!ssu::StableComp::eq(univSum(i), referenceSum(i), 3e-9))
        ::std::cout << "Diff: " << univSum(i) - referenceSum(i) << ::std::endl;
#endif

      BOOST_REQUIRE(ssu::StableComp::eq(orthoSum(i), referenceSum(i), 3e-9));
      BOOST_REQUIRE(ssu::StableComp::eq(univSum(i), referenceSum(i), 3e-9));
    }
  }

#ifdef DIST_TEST_MANUAL_DEBUG
  ::std::cout << "Univ. took: " << tUniv << ::std::endl;
  ::std::cout << "Ortho took: " << tOrtho << ::std::endl;
#endif
}

BOOST_AUTO_TEST_CASE(NonOrthogonalComparison)
{
  // SETTINGS ////////////////
  const double cellDim = 1;
  const double tolerance = 1e-13;
#ifdef DIST_TEST_MANUAL_DEBUG
  const size_t numAtoms = 15;
  const double maxCutoff = 2 * cellDim;
#else
  const size_t numAtoms = 50;
  const double maxCutoff = 4 * cellDim;
#endif
  const size_t numAttempts = 100;

  ::ssbc::RandomUnitCell randomCell;

  double cutoff;
  ::std::vector<double> referenceDists, univDist;
  ::std::vector< ::arma::vec3> referenceVecs, univVecs;
  ::arma::vec3 referenceSum, univSum;
  ::arma::vec3 A, B, C;
  for(size_t attempt = 0; attempt < numAttempts; ++attempt)
  {
    referenceDists.clear(); referenceVecs.clear();
    univDist.clear(); univVecs.clear();

    ssc::Structure structure;
    structure.setUnitCell(randomCell.generateCell());

    for(size_t i = 0; i < numAtoms; ++i)
      structure.newAtom(ssc::AtomSpeciesId::CUSTOM_1).setPosition(structure.getUnitCell()->randomPoint());

    ssc::UniversalCrystalDistanceCalculator univCalc(structure);
    ssc::ReferenceDistanceCalculator referenceCalc(structure);

    BOOST_REQUIRE(univCalc.isValid());
    BOOST_REQUIRE(referenceCalc.isValid());

    cutoff = ssc::randDouble() * maxCutoff;

    for(size_t i = 0; i < numAtoms; ++i)
    {
      const ssc::Atom atom1 = structure.getAtom(i);
      for(size_t j = i; j < numAtoms; ++j)
      {
        const ssc::Atom atom2 = structure.getAtom(j);
        univCalc.getDistsBetween(atom1, atom2, cutoff, univDist);
        univCalc.getVecsBetween(atom1, atom2, cutoff, univVecs);
        referenceCalc.getDistsBetween(atom1, atom2, cutoff, referenceDists);
        referenceCalc.getVecsBetween(atom1, atom2, cutoff, referenceVecs);
      }
    }

    referenceDists.reserve(referenceVecs.size());
    for(size_t i = 0; i < referenceVecs.size(); ++i)
    {
      referenceDists[i] = sqrt(::arma::dot(referenceVecs[i], referenceVecs[i]));
    }



#ifdef DIST_TEST_MANUAL_DEBUG
    if(referenceVecs.size() != univVecs.size() || referenceDists.size() != univDist.size())
      ::std::cout << "Vector size mismatch.\n";
#endif

    BOOST_REQUIRE(referenceDists.size() == univDist.size());
    BOOST_REQUIRE(referenceVecs.size() == univVecs.size());

    ::std::sort(referenceDists.begin(), referenceDists.end());
    ::std::sort(univDist.begin(), univDist.end());

    const size_t numElements = ::std::min(referenceDists.size(), univDist.size());

    univSum.zeros();
    referenceSum.zeros();
    for(size_t i = 0; i < numElements; ++i)
    {
      referenceSum += referenceVecs[i];
      univSum += univVecs[i];

#ifdef DIST_TEST_MANUAL_DEBUG
      if(!ssu::StableComp::eq(referenceDists[i], univDist[i], tolerance))
        ::std::cout << "Diff in distance (refernce - universal): " << referenceDists[i] - univDist[i] << ::std::endl;
#endif

      BOOST_REQUIRE(ssu::StableComp::eq(referenceDists[i], univDist[i], tolerance));
    }
    // Check that the components of the sum of the vectors match
    for(size_t i = 0; i < 3; ++i)
    {

#ifdef DIST_TEST_MANUAL_DEBUG
      if(!ssu::StableComp::eq(referenceSum(i), univSum(i), 3e-9))
        ::std::cout << "Diff. in vector sum: (refernce - universal)" << referenceSum(i) - univSum(i) << ::std::endl;
#endif

      BOOST_REQUIRE(ssu::StableComp::eq(referenceSum(i), univSum(i), 3e-9));
    }
  }
}


BOOST_AUTO_TEST_CASE(DistanceComparisonPathological)
{
  // SETTINGS ////////////////
  const size_t numAtoms = 4;
  const double cellDim = 3.1032302270780758;
  const double tolerance = 1e-10;
  const double cutoffDist = 5.00;

  ssc::Structure structure;
//  ssc::UnitCellPtr cell(new ssc::UnitCell(cellDim, cellDim, 3.1032310973902493, 89.999998840060258, 90.000001159939757, 89.999999999947718));
  ssc::UnitCellPtr cell(new ssc::UnitCell(cellDim, cellDim, 3.1032310973902493, 89.9999999999, 90.0, 90.0));

  structure.setUnitCell(cell);

  // Put in four atoms
  structure.newAtom(ssc::AtomSpeciesId::NA).setPosition(-1.6581022716893747, -2.4672347213487633, -1.3431304885983484);
  structure.newAtom(ssc::AtomSpeciesId::NA).setPosition(-3.0839601555007641, -0.79937989707630863, -1.3574676571980899);
  structure.newAtom(ssc::AtomSpeciesId::NA).setPosition(-3.0355997292334425, -2.1869067011837431, 0.34178278728968065);
  structure.newAtom(ssc::AtomSpeciesId::NA).setPosition(-1.3834008907850099, -0.77711917814879217, 0.029117512324985273);


  ssc::ReferenceDistanceCalculator referenceCalc(structure);
  ssc::OrthoCellDistanceCalculator orthoCalc(structure);
  ssc::UniversalCrystalDistanceCalculator univCalc(structure);
  

  BOOST_REQUIRE(referenceCalc.isValid());
  BOOST_REQUIRE(orthoCalc.isValid());
  BOOST_REQUIRE(univCalc.isValid());

  ::std::vector<double> orthoDist, univDist, referenceDists;

  for(size_t i = 0; i < numAtoms; ++i)
  {
    const ssc::Atom atom1 = structure.getAtom(i);
    for(size_t j = i; j < numAtoms; ++j)
    {
      const ssc::Atom atom2 = structure.getAtom(j);
      referenceCalc.getDistsBetween(atom1, atom2, cutoffDist, referenceDists, 10000);
      univCalc.getDistsBetween(atom1, atom2, cutoffDist, univDist, 10000);
      orthoCalc.getDistsBetween(atom1, atom2, cutoffDist, orthoDist, 10000);
    }
  }

  BOOST_REQUIRE(orthoDist.size() == referenceDists.size());
  BOOST_REQUIRE(univDist.size() == referenceDists.size());

  // Sort all the distances
  ::std::sort(referenceDists.begin(), referenceDists.end());
  ::std::sort(orthoDist.begin(), orthoDist.end());
  ::std::sort(univDist.begin(), univDist.end());

  const size_t numElements = ::std::min(orthoDist.size(), univDist.size());

  for(size_t i = 0; i < numElements; ++i)
  {
#ifdef DIST_TEST_MANUAL_DEBUG
    if(!ssu::StableComp::eq(orthoDist[i], referenceDists[i], tolerance))
      ::std::cout << "Diff. in distance (ortho - reference): " << orthoDist[i] - referenceDists[i] << ::std::endl;
    if(!ssu::StableComp::eq(univDist[i], referenceDists[i], tolerance))
      ::std::cout << "Diff. in distance (univ - reference): " << univDist[i] - referenceDists[i] << ::std::endl;
#endif
    BOOST_REQUIRE(ssu::StableComp::eq(orthoDist[i], referenceDists[i], tolerance));
    BOOST_REQUIRE(ssu::StableComp::eq(univDist[i], referenceDists[i], tolerance));
  }
}
