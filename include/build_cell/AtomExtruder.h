/*
 * AtomExtruder.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef ATOM_EXTRUDER_H
#define ATOM_EXTRUDER_H

// INCLUDES ////////////
#include "build_cell/StructureDescriptionVisitor.h"

#include <vector>

#include <armadillo>

#include "common/Types.h"

namespace sstbx {
namespace common {
  class DistanceCalculator;
  class Structure;
}
  
namespace build_cell {

class AtomExtruder
{
public:

  static const double DEFAULT_TOLERANCE;
  static const size_t DEFAULT_MAX_ITERATIONS;

  bool extrudeAtoms(
    common::Structure & structure,
    const size_t maxIterations = DEFAULT_MAX_ITERATIONS,
    const double tolerance = DEFAULT_TOLERANCE) const;

private:

  bool extrudeAtoms(
    const common::DistanceCalculator & distanceCalc,
    ::std::vector<common::Atom *> & atoms,
    const ::arma::mat & sepSqMtx,
    const double tolerance,
    const size_t maxIterations) const;

};

}
}

#endif /* ATOM_EXTRUDER_H */
