/*
 * OptimisationSettings.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef OPTIMISATION_SETTINGS_H
#define OPTIMISATION_SETTINGS_H

// INCLUDES /////////////////////////////////////////////

#include <vector>

#include <boost/ptr_container/ptr_list.hpp>

#include <armadillo>

#include "common/Types.h"
#include "potential/OptimisationConstraint.h"

// DEFINES //////////////////////////////////////////////

namespace sstbx {
namespace potential {

// FORWARD DECLARATIONS ////////////////////////////////////
class OptimisationConstraint;

class OptimisationSettings
{
public:

  typedef ::boost::ptr_list<OptimisationConstraint> Constraints;
  typedef Constraints::iterator ConstraintsIterator;
  typedef Constraints::const_iterator ConstConstraintsIterator;
  typedef ::boost::optional<unsigned int> OptionalUint;

  enum Optimise
  {
    ATOMS             = 0x01, // 000
    LATTICE           = 0x02, // 010
    ATOMS_AND_LATTICE = 0x03  // 011
  };

  OptimisationSettings();
  OptimisationSettings(const Optimise optimise_);
  OptimisationSettings(const Optimise optimise_, const ::arma::mat33 & externalPressure_);

  void setExternalPressure(const ::arma::mat33 & pressure);
  const ::arma::mat33 & getExternalPressure() const;

  void setOptimise(const Optimise & optimise);
  Optimise getOptimise() const;

  void setMaxIterations(const OptionalUint & maxIters);
  OptionalUint getMaxIterations() const;

  void insertConstraint(OptimisationConstraint & constraint);
  ConstraintsIterator beginConstraints();
  ConstraintsIterator endConstraints();
  ConstraintsIterator eraseConstraint(ConstraintsIterator it);

  void applyLatticeConstraints(
    const common::Structure & structure,
    const ::arma::mat33 & lattice,
    ::arma::mat33 & deltaLattice) const;

  void applyAtomsConstraints(
    const common::Structure & structure,
    const ::arma::mat & currentPos,
    ::arma::mat & deltaPos) const;


private:

  Optimise myOptimise;
  ::arma::mat33 myExternalPressure;
  Constraints myConstraints;
  OptionalUint myMaxIterations;

};


}
}

#endif /* OPTIMISATION_SETTINGS_H */
