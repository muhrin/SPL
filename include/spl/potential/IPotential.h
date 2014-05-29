/*
 * IPotential.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef SPL__POTENTIAL__POTENTIAL_H_
#define SPL__POTENTIAL__POTENTIAL_H_

// INCLUDES /////////////////////////////////////////////
#include "spl/SSLib.h"

#include <boost/optional.hpp>
#include <boost/smart_ptr.hpp>

#include "spl/common/AtomSpeciesId.h"
#include "spl/potential/PotentialData.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace common {
class AtomSpeciesDatabase;
class Structure;
}
namespace potential {

class IParameterisable;
class IPotentialEvaluator;

class IPotential
{
public:
  typedef UniquePtr< IPotentialEvaluator>::Type EvaluatorPtr;

  virtual
  ~IPotential()
  {
  }

  virtual ::boost::optional< double>
  getSpeciesPairDistance(const SpeciesPair & pair) const = 0;

  virtual EvaluatorPtr
  createEvaluator(const spl::common::Structure & structure) const = 0;

  virtual IParameterisable *
  getParameterisable() = 0;

  virtual bool
  updateSpeciesDb(common::AtomSpeciesDatabase * const speciesDb) const = 0;
};

}
}

#endif /* SPL__POTENTIAL__POTENTIAL_H */
