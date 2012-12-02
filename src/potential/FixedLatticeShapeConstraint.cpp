/*
 * FixedLatticeShapeConstraint.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "potential/FixedLatticeShapeConstraint.h"

#include "SSLibAssert.h"
#include "common/Structure.h"
#include "common/UnitCell.h"
#include "utility/IndexingEnums.h"

// NAMESPACES ////////////////////////////////


namespace sstbx {
namespace potential {

void FixedLatticeShapeConstraint::constrainLattice(
  const common::Structure & structure,
  const ::arma::mat33 & lattice,
  ::arma::mat33 & deltaLattice) const
{
  using namespace utility::cell_params_enum;

  const common::UnitCell * const cell = structure.getUnitCell();

  SSLIB_ASSERT_MSG(cell, "Error: Lattice is being constrained for structure with no unit cell.");

  const ::arma::mat33 newLattice = lattice + deltaLattice;
  const double (&params)[6] = cell->getLatticeParams();
  double meanScaleFactor = 0.0;
  for(size_t i = A; i <= C; ++i)
  {
    meanScaleFactor += sqrt(::arma::dot(newLattice.col(i), newLattice.col(i))) / params[i];
  }
  meanScaleFactor *= 1.0 / 3.0; // Normalise

  // Finally set the delta to be the best that we can achieve within the constraint
  deltaLattice = (meanScaleFactor - 1) * lattice;
}

OptimisationConstraint *
FixedLatticeShapeConstraint::doClone() const
{
  return new FixedLatticeShapeConstraint(*this);
}

}
}

