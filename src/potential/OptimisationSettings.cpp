/*
 * OptimisationSettings.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "potential/OptimisationSettings.h"

#include <boost/foreach.hpp>

// NAMESPACES ////////////////////////////////


namespace sstbx {
namespace potential {

OptimisationSettings::OptimisationSettings():
myOptimise(ATOMS_AND_LATTICE)
{
  myExternalPressure.zeros();
}

OptimisationSettings::OptimisationSettings(const Optimise optimise_):
myOptimise(optimise_),
myExternalPressure(::arma::zeros< ::arma::mat>(3, 3))
{}

OptimisationSettings::OptimisationSettings(const Optimise optimise_, const ::arma::mat33 & externalPressure_):
myOptimise(optimise_),
myExternalPressure(externalPressure_)
{}

void OptimisationSettings::setExternalPressure(const ::arma::mat33 & pressure)
{
  myExternalPressure = pressure;
}

const ::arma::mat33 & OptimisationSettings::getExternalPressure() const
{
  return myExternalPressure;
}

void OptimisationSettings::setOptimise(const Optimise & optimise)
{
  myOptimise = optimise;
}

OptimisationSettings::Optimise OptimisationSettings::getOptimise() const
{
  return myOptimise;
}

void
OptimisationSettings::insertConstraint(OptimisationConstraint & constraint)
{
  myConstraints.push_back(constraint.clone().release());
}

OptimisationSettings::ConstraintsIterator
OptimisationSettings::beginConstraints()
{
  return myConstraints.begin();
}

OptimisationSettings::ConstraintsIterator
OptimisationSettings::endConstraints()
{
  return myConstraints.end();
}

OptimisationSettings::ConstraintsIterator
OptimisationSettings::eraseConstraint(ConstraintsIterator it)
{
  return myConstraints.erase(it);
}

void OptimisationSettings::applyLatticeConstraints(
  const common::Structure & structure,
  const ::arma::mat33 & lattice,
  ::arma::mat33 & deltaLattice) const
{
  BOOST_FOREACH(const OptimisationConstraint & constraint, myConstraints)
  {
    constraint.constrainLattice(structure, lattice, deltaLattice);
  }
}

void OptimisationSettings::applyAtomsConstraints(
  const common::Structure & structure,
  const ::arma::mat & currentPos,
  ::arma::mat & deltaPos) const
{
  BOOST_FOREACH(const OptimisationConstraint & constraint, myConstraints)
  {
    constraint.constrainAtoms(structure, currentPos, deltaPos);
  }
}

}
}

