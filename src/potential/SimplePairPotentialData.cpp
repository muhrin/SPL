/*
 * SimplePairPotentialData.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "potential/SimplePairPotentialData.h"



// NAMESPACES ////////////////////////////////


namespace sstbx {
namespace potential {

SimplePairPotentialData::SimplePairPotentialData(
  const sstbx::common::Structure & structure,
  const SimplePairPotentialData::SpeciesList & speciesList):
PotentialData(structure)
{
  using sstbx::common::AtomSpeciesId;

	// Get the atom species
  std::vector<AtomSpeciesId::Value> strSpecies;
	structure.getAtomSpecies(strSpecies);

	// Now populate our species vector
  const size_t numAtoms = strSpecies.size();
	species.resize(numAtoms);

  bool found;
  const size_t numSpecies = speciesList.size();
  AtomSpeciesId::Value currentSpecies;
  for(size_t i = 0; i < numAtoms; ++i)
  {
    currentSpecies = strSpecies[i];
    found = false;
    for(size_t j = 0; j < numSpecies; ++j)
    {
      if(currentSpecies == speciesList[j])
      {
        found = true;
        species[i] = j;
        break;
      }
    }
    if(!found)
    {
      species[i] = IGNORE_ATOM;
    }
  }

}


}
}

