/*
 * LennardJonesTest.cpp
 *
 *  Created on: May 14, 2014
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "sslibtest.h"

#include <vector>

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

  std::string (*toString)(const double & arg) = boost::lexical_cast<std::string>;

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

  potential::PotentialData potData;

  BOOST_REQUIRE(lj.evaluate(structure, potData));
  BOOST_CHECK_CLOSE(potData.internalEnergy, -EPSILON, TOL);

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

  // Combining rules
  params.push_back("none");
  params.push_back("none");

  std::string err;
  lj.setParams(params, &err);

  BOOST_CHECK(params == lj.getParams());
  std::vector< std::string> ljParams = lj.getParams();
  BOOST_FOREACH(const std::string & x, ljParams)
    std::cout << x << "\n";

  // Build up tetrahedron of particles
  structure.newAtom("B").setPosition(0.5 * R_0, sqrt(3) / 2.0 * R_0, 0.0);
  structure.newAtom("B").setPosition(0.5 * R_0, sqrt(3) / 6.0 * R_0,
      sqrt(6) / 3.0 * R_0);

  potData.reset();
  BOOST_REQUIRE(lj.evaluate(structure, potData));
  BOOST_CHECK_CLOSE(potData.internalEnergy, 6.0 * -EPSILON, TOL);
}

BOOST_AUTO_TEST_SUITE_END()
