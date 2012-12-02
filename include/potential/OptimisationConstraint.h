/*
 * OptimisationConstraint.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef OPTIMISATION_CONSTRAINT_H
#define OPTIMISATION_CONSTRAINT_H

// INCLUDES /////////////////////////////////////////////

#include <armadillo>

#include "SSLibTypes.h"

// DEFINES //////////////////////////////////////////////


namespace sstbx {

// FORWARD DECLARATIONS ////////////////////////////////////
namespace common {
class Structure;
}
namespace potential {

class OptimisationConstraint
{
public:

  typedef UniquePtr<OptimisationConstraint>::Type OptimisationConstraintPtr;

  virtual ~OptimisationConstraint() {}

  virtual OptimisationConstraintPtr clone() const
  { return OptimisationConstraintPtr(doClone()); }

  virtual void constrainAtoms(
    const common::Structure & structure,
    const ::arma::mat & currentPos,
    ::arma::mat & deltaPos) const
  {}

  virtual void constrainLattice(
    const common::Structure & structure,
    const ::arma::mat33 & lattice,
    ::arma::mat33 & deltaLattice) const
  {}
private:

  virtual OptimisationConstraint * doClone() const = 0;
};

inline
OptimisationConstraint * new_clone(const OptimisationConstraint & constraint)
{
  return constraint.clone().release();
}

}
}

#endif /* OPTIMISATION_CONSTRAINT_H */
