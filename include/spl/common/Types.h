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
#include "spl/SSLibTypes.h"

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include "spl/utility/Range.h"

// DEFINES ////////////////////////////////////////////////

namespace spl {
namespace common {

// FORWARD DECLARATIONS ////////////////////////////////////
class Atom;
class DistanceCalculator;
class Structure;
class UnitCell;

typedef utility::OrderedPair< std::string> SpeciesPair;

typedef UniquePtr<Structure>::Type StructurePtr;

typedef UniquePtr<UnitCell>::Type UnitCellPtr;

typedef UniquePtr<DistanceCalculator>::Type DistanceCalculatorPtr;

namespace types {

typedef UniquePtr<Structure>::Type StructurePtr;

typedef UniquePtr<UnitCell>::Type UnitCellPtr;

typedef UniquePtr<DistanceCalculator>::Type DistanceCalculatorPtr;

}
}
}

#endif /* COMMON__TYPES_H */
