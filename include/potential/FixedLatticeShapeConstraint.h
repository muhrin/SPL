/*
 * FixedLatticeShapeConstraint.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef FIXED_LATTICE_SHAPE_CONSTRAINT_H
#define FIXED_LATTICE_SHAPE_CONSTRAINT_H

// INCLUDES /////////////////////////////////////////////
#include "potential/OptimisationConstraint.h"

// DEFINES //////////////////////////////////////////////


namespace sstbx {
namespace potential {

class FixedLatticeShapeConstraint : public OptimisationConstraint
{
public:

  virtual void constrainLattice(
    const common::Structure & structure,
    const ::arma::mat33 & lattice,
    ::arma::mat33 & deltaLattice) const;

private:
  virtual OptimisationConstraint * doClone() const;

};

}
}

#endif /* FIXED_LATTICE_SHAPE_CONSTRAINT_H */
