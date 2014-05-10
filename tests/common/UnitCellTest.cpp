/*
 * UnitCellTest.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "sslibtest.h"

#include <boost/shared_ptr.hpp>

#include <armadillo>

#include <spl/build_cell/GenerationOutcome.h>
#include <spl/build_cell/RandomUnitCellGenerator.h>
#include <spl/common/UnitCell.h>

namespace ssbc = spl::build_cell;
namespace ssc = spl::common;

BOOST_AUTO_TEST_SUITE(UnitCell)

BOOST_AUTO_TEST_CASE(Volume)
{
  spl::common::UnitCell cell(2.0, 4.0, 8.0, 90.0, 90.0, 90.0);
  BOOST_CHECK_CLOSE_FRACTION(cell.getVolume(), 2.0 * 4.0 * 8.0, 0.0001);
  BOOST_CHECK_LE(cell.getNormVolume(), 1.0);

  cell.setVolume(235.87);
  BOOST_CHECK_CLOSE_FRACTION(cell.getVolume(), 235.87, 0.0001);
}

BOOST_AUTO_TEST_CASE(LongestDiagonal)
{
  const size_t numUnitCells = 100;

  ssbc::RandomUnitCellGenerator cellGen;

  arma::vec3 A, B, C, combination, longest, diag;
  double lengthSq, maxLengthSq;
  for(size_t i = 0; i < numUnitCells; ++i)
  {
    ssc::UnitCellPtr cell;
    BOOST_REQUIRE(cellGen.generateCell(cell).isSuccess());

    // Get the longest cell vector
    A = cell->getAVec();
    B = cell->getBVec();
    C = cell->getCVec();

    longest = A + B + C;
    maxLengthSq = arma::dot(longest, longest);

    combination = A + B - C;
    lengthSq = arma::dot(combination, combination);
    if(lengthSq > maxLengthSq)
    {
      longest = combination;
      maxLengthSq = lengthSq;
    }

    combination = A - B + C;
    lengthSq = arma::dot(combination, combination);
    if(lengthSq > maxLengthSq)
    {
      longest = combination;
      maxLengthSq = lengthSq;
    }

    combination = A - B - C;
    lengthSq = arma::dot(combination, combination);
    if(lengthSq > maxLengthSq)
    {
      longest = combination;
      maxLengthSq = lengthSq;
    }

    diag = cell->getLongestDiagonal();

    BOOST_CHECK_CLOSE_FRACTION(maxLengthSq, arma::dot(diag, diag), 0.0001);
  }
}

BOOST_AUTO_TEST_SUITE_END()
