/*
 * LennardJonesTest.cpp
 *
 *  Created on: May 14, 2014
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "sslibtest.h"

#include <vector>
#include <fstream>

#include <boost/lexical_cast.hpp>

#include <armadillo>

#include <spl/common/Structure.h>
#include <spl/potential/LennardJones.h>

// NAMESPACES ///////////////////////////////
using namespace spl;

BOOST_AUTO_TEST_SUITE(LennardJones)

BOOST_AUTO_TEST_CASE(Energy)
{
  using std::sqrt;
  using boost::lexical_cast;

  std::string
  (*toString)(const double & arg) = boost::lexical_cast<std::string>;

  static const double EPSILON = 1.0;
  static const double SIGMA = 1.0;
  static const double BETA = 1.0;
  // Set the cutoff to (basically) infinity so that we can
  // use the 'exact' values for the energy i.e. -EPSILON
  static const double CUTOFF = std::numeric_limits< double>::max();
  static const double M = 12;
  static const double N = 6;
  // Equilibrium separation
  static const double R_0 = std::pow(2.0, 1.0 / 6.0);
  static const double TOL = 1e-10;

  common::Structure structure;
  // Two particles, at equilibrium separation
  structure.newAtom("A").setPosition(0.0, 0.0, 0.0);
  structure.newAtom("A").setPosition(R_0, 0.0, 0.0);

  potential::LennardJones lj;
  lj.addInteraction(SpeciesPair("A"), EPSILON, SIGMA, M, N, CUTOFF);

  {
    potential::PotentialData potData(structure.getNumAtoms());
    BOOST_REQUIRE(lj.evaluate(structure, potData));
    BOOST_CHECK_CLOSE(potData.internalEnergy, -EPSILON, TOL);
  }

  // Use parameter list to set the interactions
  std::vector< std::string> params;
  params.push_back("A~A");
  params.push_back((*toString)(EPSILON));
  params.push_back((*toString)(SIGMA));
  params.push_back((*toString)(M));
  params.push_back((*toString)(N));
  params.push_back((*toString)(CUTOFF));

  params.push_back("A~B");
  params.push_back((*toString)(EPSILON));
  params.push_back((*toString)(SIGMA));
  params.push_back((*toString)(M));
  params.push_back((*toString)(N));
  params.push_back((*toString)(CUTOFF));

  params.push_back("B~B");
  params.push_back((*toString)(EPSILON));
  params.push_back((*toString)(SIGMA));
  params.push_back((*toString)(M));
  params.push_back((*toString)(N));
  params.push_back((*toString)(CUTOFF));

  std::string err;
  BOOST_REQUIRE(lj.setParams(params, &err));

  BOOST_CHECK(params == lj.getParams());

  // Build up tetrahedron of particles
  structure.newAtom("B").setPosition(0.5 * R_0, sqrt(3) / 2.0 * R_0, 0.0);
  structure.newAtom("B").setPosition(0.5 * R_0, sqrt(3) / 6.0 * R_0,
      sqrt(6) / 3.0 * R_0);

  {
    potential::PotentialData potData(structure.getNumAtoms());
    BOOST_REQUIRE(lj.evaluate(structure, potData));
    BOOST_CHECK_CLOSE(potData.internalEnergy, 6.0 * -EPSILON, TOL);
  }
}

BOOST_AUTO_TEST_CASE(PotentialProfile)
{
  static const double NUM_SAMPLES = 300;
  static const double SAMPLE_SPACING = 0.01;
  static const double CUTOFF = 2.5;

  potential::LennardJones lj;
  lj.addInteraction(SpeciesPair("A"), 1.0, 1.0, 12, 6, CUTOFF);

  common::Structure structure;
  // Create two atoms at the origin
  structure.newAtom("A");
  common::Atom & atom = structure.newAtom("A");

  std::ofstream os("lj_profile.dat");

  potential::PotentialData potData(structure.getNumAtoms());
  for(int i = 1; i < NUM_SAMPLES; ++i)
  {
    const double r = static_cast< double>(i) * SAMPLE_SPACING;
    atom.setPosition(r, 0.0, 0.0);
    lj.evaluate(structure, potData);

    os << r << " " << potData.internalEnergy << "\n";
    if(r >= CUTOFF)
      BOOST_CHECK_EQUAL(potData.internalEnergy, 0.0);

    potData.reset();
  }

  os.close();
}

BOOST_AUTO_TEST_SUITE_END()
