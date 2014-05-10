/*
 * AtomSpeciesDatabase.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef ATOM_SPECIES_DATABASE_H
#define ATOM_SPECIES_DATABASE_H

// INCLUDES /////////////////////////////////////////////
#include <map>
#include <string>

#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include "spl/common/AtomSpeciesId.h"
#include "spl/common/Types.h"
#include "spl/OptionalTypes.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace common {

class AtomSpeciesDatabase
{
public:
  AtomSpeciesDatabase();

  void
  setAll(AtomSpeciesId::Value id, const std::string & name,
      const double radius);

  const std::string *
  getName(const AtomSpeciesId::Value id) const;
  void
  setName(const AtomSpeciesId::Value id, const std::string & name);

  OptionalDouble
  getRadius(const AtomSpeciesId::Value id) const;
  void
  setRadius(const AtomSpeciesId::Value id, const double radius);

  OptionalDouble
  getSpeciesPairDistance(const SpeciesPair & pair) const;
  void
  setSpeciesPairDistance(const SpeciesPair & pair, const double dist);

protected:
  typedef std::map< AtomSpeciesId::Value, std::string> SpeciesString;
  typedef std::map< AtomSpeciesId::Value, double> SpeciesDouble;
  typedef std::map< SpeciesPair, double> SpeciesPairDistances;

  SpeciesString myNames;
  SpeciesDouble myRadii;
  SpeciesPairDistances mySpeciesPairDistances;
};

}
}

#endif /* ATOM_SPECIES_DATABASE_H */
