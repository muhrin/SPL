/*
 * AtomsDescription.h
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#ifndef ATOMS_DESCRIPTION_H
#define ATOMS_DESCRIPTION_H

// INCLUDES ///////////////////

#include <string>

#include "spl/OptionalTypes.h"
#include "spl/common/AtomSpeciesId.h"
#include "spl/utility/Range.h"

// FORWARD DECLARES ///////////

namespace spl {
namespace build_cell {

struct AtomsDescription
{
  typedef utility::Range< int> CountRange;

  AtomsDescription(const common::AtomSpeciesId::Value _species,
      const int _count = 1) :
      species(_species), count(_count, _count), label(_species)
  {
  }
  AtomsDescription(const common::AtomSpeciesId::Value _species,
      const CountRange _count) :
      species(_species), count(_count), label(_species)
  {
  }

  common::AtomSpeciesId::Value species;
  CountRange count;
  std::string label;
  OptionalDouble radius;
  OptionalArmaVec3 position;
};

}
}

#endif /* ATOMS_DESCRIPTION_H */
