/*
 * DistanceCalculatorsTest.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "sslibtest.h"

#include <armadillo>

#include <spl/analysis/SpaceGroup.h>
#include <spl/common/Structure.h>
#include <spl/io/ResReaderWriter.h>

using namespace spl;

BOOST_AUTO_TEST_SUITE(SpaceGroup)

BOOST_AUTO_TEST_CASE(EmptyStructure)
{
  common::Structure structure;
  analysis::space_group::SpacegroupInfo info;
  BOOST_CHECK(!analysis::space_group::getSpacegroupInfo(info, structure));
}

BOOST_AUTO_TEST_CASE(NoUnitCell)
{
  common::Structure structure;
  structure.newAtom("Na").setPosition(arma::randu(3));
  analysis::space_group::SpacegroupInfo info;
  BOOST_CHECK(!analysis::space_group::getSpacegroupInfo(info, structure));
}

BOOST_AUTO_TEST_CASE(NoAtoms)
{
  common::Structure structure;
  structure.setUnitCell(common::UnitCell());
  analysis::space_group::SpacegroupInfo info;
  BOOST_CHECK(!analysis::space_group::getSpacegroupInfo(info, structure));
}

BOOST_AUTO_TEST_CASE(SpgTest)
{
  io::ResReaderWriter resReader;
  UniquePtr<common::Structure>::Type structure =
      resReader.readStructure(io::ResourceLocator("Rutile.res"));
  BOOST_REQUIRE(structure.get());

  analysis::space_group::SpacegroupInfo info;
  BOOST_REQUIRE(analysis::space_group::getSpacegroupInfo(info, *structure));
  BOOST_CHECK_EQUAL(info.number, 136);
}

BOOST_AUTO_TEST_SUITE_END()
