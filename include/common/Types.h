/*
 * Types.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef COMMON__TYPES_H
#define COMMON__TYPES_H

// INCLUDES /////////////////////////////////////////////
#include "SSLibTypes.h"

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <armadillo>

// DEFINES ////////////////////////////////////////////////

namespace sstbx {
namespace common {

// FORWARD DECLARATIONS ////////////////////////////////////
class Atom;
class DistanceCalculator;
class Structure;
class UnitCell;

typedef UniquePtr<Structure>::Type StructurePtr;

typedef UniquePtr<UnitCell>::Type UnitCellPtr;

typedef UniquePtr<DistanceCalculator>::Type DistanceCalculatorPtr;

typedef ::boost::optional<const ::arma::mat33> OptionalConstMat33;

}
}

#endif /* COMMON__TYPES_H */
