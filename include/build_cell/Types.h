/*
 * Types.h
 *
 *  Created on: Aug 30, 2012
 *      Author: Martin Uhrin
 */

#ifndef BUILD_CELL__TYPES_H
#define BUILD_CELL__TYPES_H

// INCLUDES ////////////
#include "SSLib.h"

namespace sstbx {
namespace build_cell {

// FORWARD DECLARES ////////////
class AtomsDescription;
class IUnitCellBlueprint;
class RandomUnitCell;
class StructureDescription;

typedef UniquePtr<AtomsDescription>::Type AtomsDescriptionPtr;

typedef UniquePtr<StructureDescription>::Type StructureDescriptionPtr;

typedef UniquePtr<IUnitCellBlueprint>::Type UnitCellBlueprintPtr;
typedef UniquePtr<const IUnitCellBlueprint>::Type ConstUnitCellBlueprintPtr;

typedef UniquePtr<RandomUnitCell>::Type RandomUnitCellPtr;

}
}

#endif /* BUILD_CELL__TYPES_H */
