/*
 * IUnitCellGenerator.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_UNIT_CELL_GENERATOR_H
#define I_UNIT_CELL_GENERATOR_H

// INCLUDES ////////////
#include "spl/build_cell/BuildCellFwd.h"
#include "spl/build_cell/GenerationOutcome.h"
#include "spl/common/Types.h"

namespace spl {
// FORWARD DECLARATIONS ///////
namespace common {
class UnitCell;
}

namespace build_cell {
class StructureContents;

class IUnitCellGenerator
{
public:
  virtual ~IUnitCellGenerator() {}

  virtual GenerationOutcome generateCell(
    common::UnitCellPtr & cellOut,
    const bool structureIsCluster = false
  ) const = 0;
  virtual GenerationOutcome generateCell(
    common::UnitCellPtr & cellOut,
    const StructureContents & contents,
    const bool structureIsCluster = false
  ) const = 0;

  virtual IUnitCellGeneratorPtr clone() const = 0;
};

}
}

#endif /* I_UNIT_CELL_GENERATOR_H */
