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
#include "spl/SSLib.h"

#include <boost/optional.hpp>
#include <boost/smart_ptr.hpp>

#include "spl/common/AtomSpeciesId.h"
#include "spl/potential/PotentialData.h"

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

  virtual ::boost::optional< double>
  getSpeciesPairDistance(const SpeciesPair & pair) const = 0;

  virtual ::boost::shared_ptr< IPotentialEvaluator>
  createEvaluator(const spl::common::Structure & structure) const = 0;

  virtual IParameterisable *
  getParameterisable() = 0;
};

}
}

#endif /* I_POTENTIAL_H */
