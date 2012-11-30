/*
 * IGeomOptimiser.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_GEOM_OPTIMISER_H
#define I_GEOM_OPTIMISER_H

// INCLUDES /////////////////////////////////////////////

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <armadillo>

#include "common/Types.h"

// DEFINES //////////////////////////////////////////////


namespace sstbx {

// FORWARD DECLARATIONS ////////////////////////////////////
namespace common {
class Structure;
}
namespace potential {
struct PotentialData;
class IPotential;

struct OptimisationOptions
{
  enum Optimise
  {
    ATOMS             = 0x01, // 000
    LATTICE           = 0x02, // 010
    ATOMS_AND_LATTICE = 0x03  // 011
  };

  OptimisationOptions():
  optimise(ATOMS_AND_LATTICE)
  {
    externalPressure.zeros();
  }

  OptimisationOptions(const Optimise optimise_):
  optimise(optimise_),
  externalPressure(::arma::zeros< ::arma::mat>(3, 3))
  {}

  OptimisationOptions(const Optimise optimise_, const ::arma::mat33 & externalPressure_):
  optimise(optimise_),
  externalPressure(externalPressure_)
  {}

  Optimise optimise;
  ::arma::mat33 externalPressure;
};

class IGeomOptimiser
{
public:

	virtual ~IGeomOptimiser() {}

  /**
  /* Get the potential being used by the geometry optimiser.  Not all
  /* geometry optimisers need to have a potential in which case NULL
  /* will be returned.
  /**/
  virtual const IPotential * getPotential() const = 0;

  // TODO: Allow return value to give some indication of the reason for the failure!
	virtual bool optimise(
    common::Structure & structure,
    const OptimisationOptions & options) const = 0;

	virtual bool optimise(
		common::Structure & structure,
    PotentialData & data,
    const OptimisationOptions & options) const = 0;
};

}
}

#endif /* I_GEOM_OPTIMISER_H */
