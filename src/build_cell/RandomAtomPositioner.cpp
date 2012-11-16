/*
 * RandomAtomPositioner.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////////
#include "build_cell/RandomAtomPositioner.h"

#include <vector>

#include <boost/foreach.hpp>

#include <armadillo>

#include "build_cell/AtomGroupDescription.h"
#include "build_cell/AtomsDescription.h"
#include "build_cell/StructureDescriptionMap.h"
#include "common/Atom.h"
#include "common/Structure.h"
#include "common/Types.h"
#include "common/UnitCell.h"

namespace sstbx {
namespace build_cell {

RandomAtomPositioner::RandomAtomPositioner(StructureDescriptionMap & descriptionMap):
myDescriptionMap(descriptionMap),
myStructure(descriptionMap.getStructure())
{
  myCurrentPosition.zeros();
}

void RandomAtomPositioner::enteringAtomGroup(const AtomGroupDescription & atomGroup)
{
  const common::UnitCell * const cell = myStructure.getUnitCell();
  if(cell)
    pushDisplacement(cell->randomPoint());
  else
  {
    ::arma::vec3 rand;
    rand.randu();
    pushDisplacement(rand);
  }
}

void RandomAtomPositioner::leavingAtomGroup(const AtomGroupDescription & atomGroup)
{
  popDisplacement();
}

bool RandomAtomPositioner::visitAtomGroup(const AtomGroupDescription & atomGroup)
{
  const common::UnitCell * const cell = myStructure.getUnitCell();

  // Place the atoms in this group
  common::Atom * atom;
  ::arma::vec3 pos;
  const StructureDescriptionMap::GroupAtomsRange range = myDescriptionMap.getAtoms(&atomGroup);
  
  ::arma::vec3 rand;
  BOOST_FOREACH(atom, range)
  {
    if(cell)
      rand = cell->randomPoint();
    else
      rand.randu();
    pos = myCurrentPosition + rand;
    
    if(cell)
     cell->wrapVecInplace(pos);
    atom->setPosition(pos);
  }

  return true;
}

void RandomAtomPositioner::pushDisplacement(const arma::vec3 &displacement)
{
  myCurrentPosition += displacement;
  myDisplacementsStack.push_back(displacement);
}

void RandomAtomPositioner::popDisplacement()
{
  const ::arma::vec3 & last = myDisplacementsStack.back();
  myCurrentPosition -= last;
  // Remove the last element
  myDisplacementsStack.pop_back();
}

}
}
