/*
 * LennardJonesData.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef LENNARD_JONES_DATA_H
#define LENNARD_JONES_DATA_H

// INCLUDES /////////////////////////////////////////////
#include "spl/common/AtomSpeciesId.h"
#include "spl/common/Structure.h"
#include "spl/potential/PotentialData.h"

// FORWARD DECLARATIONS ////////////////////////////////////

// DEFINITION //////////////////////////////////////////////

namespace spl {
namespace potential {

struct SimplePairPotentialData : public PotentialData
{
  // A list from 0 to N of the species that each row (and column) of the parameter
  // matrices corresponds to.  The entries should be unique.
  //                      Na Cl
  // epsilon_{ij} = Na (  1  0.5 )
  //                CL ( 0.5  1  )
  // speciesMap(0 => Na, 1 => Cl)
  typedef ::std::vector< common::AtomSpeciesId::Value> SpeciesList;

  // Any atoms that are not being considered by the potential will be labelled with this.
  static const int IGNORE_ATOM = -1;

  SimplePairPotentialData(const spl::common::Structure & structure,
      const SpeciesList & speciesList);

  std::vector< int> species;
};

}
}

#endif /* LENNARD_JONES_DATA_H */

