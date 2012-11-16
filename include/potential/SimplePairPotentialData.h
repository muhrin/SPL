/*
 * SimplePairPotentialData.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef SIMPLE_PAIR_POTENTIAL_DATA_H
#define SIMPLE_PAIR_POTENTIAL_DATA_H

// INCLUDES /////////////////////////////////////////////

#include "common/AtomSpeciesId.h"
#include "common/Structure.h"
#include "potential/PotentialData.h"

// FORWARD DECLARATIONS ////////////////////////////////////


// DEFINITION //////////////////////////////////////////////

namespace sstbx {
namespace potential {

struct SimplePairPotentialData : public PotentialData
{
  /**
  /* A list from 0 to N of the species that each row (and column) of the parameter
  /* matrices corresponds to.  The entries should be unique.
  /*                      Na Cl
  /* epsilon_{ij} = Na (  1  0.5 )
  /*                CL ( 0.5  1  )
  /* speciesMap(0 => Na, 1 => Cl)
  /**/
  typedef ::std::vector< ::sstbx::common::AtomSpeciesId::Value>  SpeciesList;

  /**
  /* Any atoms that are not being considered by the potential will be labelled with this.
  /**/
  static const int IGNORE_ATOM = -1;

	SimplePairPotentialData(
    const sstbx::common::Structure & structure,
    const SpeciesList &              speciesList);

	std::vector<int>				species;

};


}
}

#endif /* SIMPLE_PAIR_POTENTIAL_DATA_H */

