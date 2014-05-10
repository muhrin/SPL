/*
 * FactoryFwd.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef FACTORY_FWD_H
#define FACTORY_FWD_H

// INCLUDES /////////////////////////////////////////////
#include <iostream>
#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include <armadillo>

#include "spl/build_cell/AtomsDescription.h"
#include "spl/common/AtomSpeciesId.h"
#include "spl/utility/HeterogeneousMap.h"
#include "spl/utility/Range.h"

// DEFINES //////////////////////////////////////////////

namespace spl {
namespace factory {

///////////////////////////////////////////////////////////
// TYPEDEFS
///////////////////////////////////////////////////////////
typedef utility::OrderedPair< std::string> SpeciesPair;
typedef std::map< SpeciesPair, double> PairDistances;

///////////////////////////////////////////////////////////
// CLASSES
///////////////////////////////////////////////////////////
struct AtomSpeciesCount
{
  AtomSpeciesCount() :
      count(1, 1)
  {
  }
  std::string species;
  build_cell::AtomsDescription::CountRange count;
};

inline std::ostream &
operator <<(std::ostream & out, const AtomSpeciesCount & speciesCount)
{
  out << speciesCount.species;
  if(!(speciesCount.count.nullSpan() && speciesCount.count.min() == 1))
    out << " " << speciesCount.count;
  return out;
}

inline std::istream &
operator >>(std::istream &in, AtomSpeciesCount & speciesCount)
{
  in >> speciesCount.species;
  if(in.good() && (in.flags() & std::ios_base::skipws) == 0)
  {
    char whitespace;
    in >> whitespace;
  }
  // The count is optional
  speciesCount.count.setBoth(1);
  if(in.good())
    in >> speciesCount.count;

  return in;
}

}
}

#endif /* FACTORY_FWD_H */

