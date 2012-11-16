/*
 * StructureBuilder.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////////
#include "build_cell/StructureBuilder.h"

#include "build_cell/AtomsDescription.h"
#include "build_cell/StructureDescription.h"
#include "build_cell/StructureDescriptionMap.h"
#include "common/Atom.h"
#include "common/AtomSpeciesDatabase.h"
#include "common/Constants.h"
#include "common/Structure.h"

namespace sstbx {
namespace build_cell {

StructureBuilder::StructureBuilder(const common::AtomSpeciesDatabase & speciesDb):
  mySpeciesDb(speciesDb)
{}

common::StructurePtr
StructureBuilder::buildStructure(const StructureDescription & description, DescriptionMapPtr & outDescriptionMap)
{
	// Create a new blank structure
  common::StructurePtr structure(new common::Structure());
  outDescriptionMap.reset(new StructureDescriptionMap(description, *structure.get()));

  myAtomsVolume = 0.0;

  myCurrentPair.first = structure.get();
  myCurrentPair.second = outDescriptionMap.get();

  description.traversePreorder(*this);

  // Reset the current structure pair
  myCurrentPair.first = NULL;
  myCurrentPair.second = NULL;

  return structure;
}

bool StructureBuilder::visitAtom(const AtomsDescription & atomDescription)
{
  const size_t numAtoms = atomDescription.getCount();
  ::boost::optional<double> optionalRadius;

  // See if the description contains a radius
  optionalRadius = atomDescription.getRadius();
  if(!optionalRadius)
  {
    // The user hasn't specified a radius so try to get a default on from the databaase
    optionalRadius = mySpeciesDb.getRadius(atomDescription.getSpecies());
  }

  double radius;
  for(size_t i = 0; i < atomDescription.getCount(); ++i)
  {
    common::Atom & atom = myCurrentPair.first->newAtom(atomDescription.getSpecies());

    if(optionalRadius)
      atom.setRadius(*optionalRadius);

    radius = atom.getRadius();

    myAtomsVolume += 1.333333 * common::Constants::PI * radius * radius * radius;

    // Finally store the atom and description pair
    myCurrentPair.second->insert(atomDescription.getParent(), &atomDescription, &atom);
  }

  return true;
}

double StructureBuilder::getAtomsVolume() const
{
  return myAtomsVolume;
}

}
}
