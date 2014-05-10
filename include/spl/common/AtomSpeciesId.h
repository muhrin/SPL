/*
 * AtomSpeciesId.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef ATOM_SPECIES_ID_H
#define ATOM_SPECIES_ID_H

// INCLUDES ////////////
#include <string>

// FORWARD DECLARATIONS ///////

namespace spl {
namespace common {

struct AtomSpeciesId
{
  // Adding this for compatibility with previous use of this type
  typedef ::std::string Value;
private:
  AtomSpeciesId() {}
};

}
}


#endif /* ATOM_SPECIES_IDS_H */
