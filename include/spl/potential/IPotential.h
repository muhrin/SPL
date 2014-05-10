/*
 * IPotential.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_POTENTIAL_H
#define I_POTENTIAL_H

// INCLUDES /////////////////////////////////////////////
#include "PotentialData.h"

#include <boost/optional.hpp>
#include <boost/smart_ptr.hpp>

#include "spl/common/AtomSpeciesId.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace common {
class Structure;
}
namespace potential {

class IParameterisable;
class IPotentialInfo;
class IPotentialEvaluator;

class IPotential
{
public:

  virtual
  ~IPotential()
  {
  }

  // Get the name of this potential.
  virtual const ::std::string &
  getName() const = 0;

  // Get the potential radius for a particular atom species.  Return value is optional as the potential may
  // not know or may not deal with the species specified.
  virtual ::boost::optional< double>
  getPotentialRadius(const ::spl::common::AtomSpeciesId::Value id) const = 0;

  virtual ::boost::optional< double>
  getSpeciesPairDistance(common::AtomSpeciesId::Value s1,
      common::AtomSpeciesId::Value s2) const = 0;

  virtual ::boost::shared_ptr< IPotentialEvaluator>
  createEvaluator(const spl::common::Structure & structure) const = 0;

  virtual IParameterisable *
  getParameterisable() = 0;
};

}
}

#endif /* I_POTENTIAL_H */
