/*
 * IParameterisable.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_PARAMETERISABLE_H
#define I_PARAMETERISABLE_H

// INCLUDES /////////////////////////////////////////////
#include <armadillo>

#include <utility>
#include <vector>

// DEFINES //////////////////////////////////////////////

namespace spl {
// FORWARD DECLARATIONS ////////////////////////////////////
namespace common {
class AtomSpeciesDatabase;
}

namespace potential {

class IParameterisable
{
public:
  typedef ::std::vector< double> PotentialParams;

  virtual
  ~IParameterisable()
  {
  }

  // Returns the number of parameters that this potential has.
  virtual size_t
  getNumParams() const = 0;

  // Get the current parameters vector for this potential.
  virtual PotentialParams
  getParams() const = 0;

  // Set the parameters for this potential.
  virtual void
  setParams(const PotentialParams & params) = 0;

  virtual bool
  updateSpeciesDb(common::AtomSpeciesDatabase * const speciesDb) const = 0;
};

}
}

#endif /* I_PARAMETERISABLE_H */
