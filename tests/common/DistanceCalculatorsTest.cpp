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

#include <spl/build_cell/GenerationOutcome.h>
#include <spl/build_cell/RandomUnitCellGenerator.h>
#include <spl/common/Atom.h>
#include <spl/common/OrthoCellDistanceCalculator.h>
#include <spl/common/ReferenceDistanceCalculator.h>
#include <spl/common/Structure.h>
#include <spl/common/Types.h>
#include <spl/common/UniversalCrystalDistanceCalculator.h>
#include <spl/common/UnitCell.h>
#include <spl/math/Random.h>
#include <spl/utility/StableComparison.h>

//#define DIST_TEST_MANUAL_DEBUG

namespace ssbc = spl::build_cell;
namespace ssc = spl::common;
namespace ssm = spl::math;
namespace ssu = spl::utility;

BOOST_AUTO_TEST_SUITE(DistanceCalculators)

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
  std::vector< double> referenceDists, orthoDist, univDist;
  std::vector< arma::vec3> referenceVecs, orthoVecs, univVecs;
  arma::vec3 orthoSum, univSum, referenceSum;
  for(size_t attempt = 0; attempt < numAttempts; ++attempt)
  {
    referenceDists.clear();
    referenceVecs.clear();
    orthoDist.clear();
    orthoVecs.clear();
    univDist.clear();
    univVecs.clear();

    ssc::Structure structure;
    structure.setUnitCell(
        ssc::UnitCell(ssm::randu(1.0, 5.0) * cellDim,
            ssm::randu(1.0, 5.0) * cellDim, ssm::randu(1.0, 5.0) * cellDim,
            90.0, 90.0, 90.0));

    for(size_t i = 0; i < numAtoms; ++i)
      structure.newAtom("C1").setPosition(
          structure.getUnitCell()->randomPoint());

    ssc::OrthoCellDistanceCalculator orthoCalc(structure.getUnitCell());
    ssc::UniversalCrystalDistanceCalculator univCalc(structure.getUnitCell());
    ssc::ReferenceDistanceCalculator referenceCalc(*structure.getUnitCell());

    BOOST_REQUIRE(orthoCalc.isValid());
    BOOST_REQUIRE(univCalc.isValid());
    BOOST_REQUIRE(referenceCalc.isValid());

    cutoff = ssm::randu(0.0, maxCutoff);

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
    std::cout << "Vector size mismatch.\n";
#endif

    BOOST_REQUIRE(orthoVecs.size() == referenceVecs.size());
    BOOST_REQUIRE(univVecs.size() == referenceVecs.size());

    std::sort(orthoDist.begin(), orthoDist.end());
    std::sort(univDist.begin(), univDist.end());
    std::sort(referenceDists.begin(), referenceDists.end());

    const size_t numElements = std::min(orthoDist.size(), univDist.size());

    referenceSum.zeros();
    univSum.zeros();
    orthoSum.zeros();
    for(size_t i = 0; i < numElements; ++i)
    {
      referenceSum += referenceVecs[i];
      orthoSum += orthoVecs[i];
      univSum += univVecs[i];

#ifdef DIST_TEST_MANUAL_DEBUG
      if(!ssu::stable::eq(orthoDist[i], referenceDists[i], tolerance))
      std::cout << "Diff. in distance (ortho - reference): " << orthoDist[i] - referenceDists[i] << std::endl;
      if(!ssu::stable::eq(univDist[i], referenceDists[i], tolerance))
      std::cout << "Diff. in distance (universal - reference)" << univDist[i] - referenceDists[i] << std::endl;
#endif

      BOOST_REQUIRE(
          ssu::stable::eq(orthoDist[i], referenceDists[i], tolerance));
      BOOST_REQUIRE(ssu::stable::eq(univDist[i], referenceDists[i], tolerance));
    }
    // Check that the components of the sum of the vectors match
    for(size_t i = 0; i < 3; ++i)
    {

#ifdef DIST_TEST_MANUAL_DEBUG
      if(!ssu::stable::eq(orthoSum(i), referenceSum(i), 3e-9))
      std::cout << "Diff: " << orthoSum(i) - referenceSum(i) << std::endl;
      if(!ssu::stable::eq(univSum(i), referenceSum(i), 3e-9))
      std::cout << "Diff: " << univSum(i) - referenceSum(i) << std::endl;
#endif

      BOOST_REQUIRE(ssu::stable::eq(orthoSum(i), referenceSum(i), 3e-9));
      BOOST_REQUIRE(ssu::stable::eq(univSum(i), referenceSum(i), 3e-9));
    }
  }

#ifdef DIST_TEST_MANUAL_DEBUG
  std::cout << "Univ. took: " << tUniv << std::endl;
  std::cout << "Ortho took: " << tOrtho << std::endl;
#endif
}

BOOST_AUTO_TEST_CASE(NonOrthogonalComparison)
{
  using std::sqrt;

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

  ssbc::RandomUnitCellGenerator randomCell;

  double cutoff;
  std::vector< double> referenceDists, univDist;
  std::vector< arma::vec3> referenceVecs, univVecs;
  arma::vec3 referenceSum, univSum;
  arma::vec3 A, B, C;
  for(size_t attempt = 0; attempt < numAttempts; ++attempt)
  {
    referenceDists.clear();
    referenceVecs.clear();
    univDist.clear();
    univVecs.clear();

    ssc::Structure structure;
    {
      ssc::UnitCellPtr cell;
      BOOST_REQUIRE(randomCell.generateCell(cell).isSuccess());
      structure.setUnitCell(*cell);
    }

    for(size_t i = 0; i < numAtoms; ++i)
      structure.newAtom("C1").setPosition(
          structure.getUnitCell()->randomPoint());

    ssc::UniversalCrystalDistanceCalculator univCalc(structure.getUnitCell());
    ssc::ReferenceDistanceCalculator referenceCalc(*structure.getUnitCell());

    BOOST_REQUIRE(univCalc.isValid());
    BOOST_REQUIRE(referenceCalc.isValid());

    cutoff = ssm::randu< double>() * maxCutoff;

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
      referenceDists[i] = sqrt(arma::dot(referenceVecs[i], referenceVecs[i]));
    }

#ifdef DIST_TEST_MANUAL_DEBUG
    if(referenceVecs.size() != univVecs.size() || referenceDists.size() != univDist.size())
    std::cout << "Vector size mismatch.\n";
#endif

    BOOST_REQUIRE(referenceDists.size() == univDist.size());
    BOOST_REQUIRE(referenceVecs.size() == univVecs.size());

    std::sort(referenceDists.begin(), referenceDists.end());
    std::sort(univDist.begin(), univDist.end());

    const size_t numElements = std::min(referenceDists.size(), univDist.size());

    univSum.zeros();
    referenceSum.zeros();
    for(size_t i = 0; i < numElements; ++i)
    {
      referenceSum += referenceVecs[i];
      univSum += univVecs[i];

#ifdef DIST_TEST_MANUAL_DEBUG
      if(!ssu::stable::eq(referenceDists[i], univDist[i], tolerance))
      std::cout << "Diff in distance (refernce - universal): " << referenceDists[i] - univDist[i] << std::endl;
#endif

      BOOST_REQUIRE(ssu::stable::eq(referenceDists[i], univDist[i], tolerance));
    }
    // Check that the components of the sum of the vectors match
    for(size_t i = 0; i < 3; ++i)
    {

#ifdef DIST_TEST_MANUAL_DEBUG
      if(!ssu::stable::eq(referenceSum(i), univSum(i), 3e-9))
      std::cout << "Diff. in vector sum: (refernce - universal)" << referenceSum(i) - univSum(i) << std::endl;
#endif

      BOOST_REQUIRE(ssu::stable::eq(referenceSum(i), univSum(i), 3e-9));
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
  const ssc::UnitCell cell(cellDim, cellDim, 3.1032310973902493, 89.9999999999,
      90.0, 90.0);

  structure.setUnitCell(cell);

  // Put in four atoms
  structure.newAtom("Na").setPosition(-1.6581022716893747, -2.4672347213487633,
      -1.3431304885983484);
  structure.newAtom("Na").setPosition(-3.0839601555007641, -0.79937989707630863,
      -1.3574676571980899);
  structure.newAtom("Na").setPosition(-3.0355997292334425, -2.1869067011837431,
      0.34178278728968065);
  structure.newAtom("Na").setPosition(-1.3834008907850099, -0.77711917814879217,
      0.029117512324985273);

  ssc::ReferenceDistanceCalculator referenceCalc(*structure.getUnitCell());
  ssc::OrthoCellDistanceCalculator orthoCalc(structure.getUnitCell());
  ssc::UniversalCrystalDistanceCalculator univCalc(structure.getUnitCell());

  BOOST_REQUIRE(referenceCalc.isValid());
  BOOST_REQUIRE(orthoCalc.isValid());
  BOOST_REQUIRE(univCalc.isValid());

  std::vector< double> orthoDist, univDist, referenceDists;

  for(size_t i = 0; i < numAtoms; ++i)
  {
    const ssc::Atom atom1 = structure.getAtom(i);
    for(size_t j = i; j < numAtoms; ++j)
    {
      const ssc::Atom atom2 = structure.getAtom(j);
      referenceCalc.getDistsBetween(atom1, atom2, cutoffDist, referenceDists,
          10000);
      univCalc.getDistsBetween(atom1, atom2, cutoffDist, univDist, 10000);
      orthoCalc.getDistsBetween(atom1, atom2, cutoffDist, orthoDist, 10000);
    }
  }

  BOOST_REQUIRE(orthoDist.size() == referenceDists.size());
  BOOST_REQUIRE(univDist.size() == referenceDists.size());

  // Sort all the distances
  std::sort(referenceDists.begin(), referenceDists.end());
  std::sort(orthoDist.begin(), orthoDist.end());
  std::sort(univDist.begin(), univDist.end());

  const size_t numElements = std::min(orthoDist.size(), univDist.size());

  for(size_t i = 0; i < numElements; ++i)
  {
#ifdef DIST_TEST_MANUAL_DEBUG
    if(!ssu::stable::eq(orthoDist[i], referenceDists[i], tolerance))
    std::cout << "Diff. in distance (ortho - reference): " << orthoDist[i] - referenceDists[i] << std::endl;
    if(!ssu::stable::eq(univDist[i], referenceDists[i], tolerance))
    std::cout << "Diff. in distance (univ - reference): " << univDist[i] - referenceDists[i] << std::endl;
#endif
    BOOST_REQUIRE(ssu::stable::eq(orthoDist[i], referenceDists[i], tolerance));
    BOOST_REQUIRE(ssu::stable::eq(univDist[i], referenceDists[i], tolerance));
  }
}

BOOST_AUTO_TEST_SUITE_END()
