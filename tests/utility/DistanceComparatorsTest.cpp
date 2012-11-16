/*
 * DistanceComparatorsTest.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/test/unit_test.hpp>

#include <build_cell/RandomUnitCell.h>
#include <common/Atom.h>
#include <common/Structure.h>
#include <common/UnitCell.h>
#include <utility/DistanceMatrixComparator.h>

namespace ssbc = ::sstbx::build_cell;
namespace ssc = ::sstbx::common;
namespace ssu = ::sstbx::utility;

BOOST_AUTO_TEST_CASE(ComparatorsComparison)
{
  // SETTINGS //////////////
  typedef ::boost::shared_ptr<ssc::UnitCell> UnitCellPtr;
  const size_t numAtoms = 10;

  ssc::Structure str1, str2;

  ssbc::RandomUnitCell randomCell;

  UnitCellPtr cell1(randomCell.generateCell());
  UnitCellPtr cell2(randomCell.generateCell());
  str1.setUnitCell(cell1);
  str2.setUnitCell(cell2);

  ssc::Atom * atom1;
  ssc::Atom * atom2;
  for(size_t i = 0; i < numAtoms; ++i)
  {
    atom1 = &str1.newAtom(ssc::AtomSpeciesId::CUSTOM_1);
    atom2 = &str2.newAtom(ssc::AtomSpeciesId::CUSTOM_1);

    atom1->setPosition(cell1->randomPoint());
    atom2->setPosition(cell2->randomPoint());
  }

  ssu::DistanceMatrixComparator distMtxComparator;

  double diff = distMtxComparator.compareStructures(str1, str2);


}

