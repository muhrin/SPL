/*
 * IUnitCellBlueprint.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_UNIT_CELL_BLUEPRINT_H
#define I_UNIT_CELL_BLUEPRINT_H

// INCLUDES ////////////
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include "common/Types.h"

namespace sstbx {
// FORWARD DECLARATIONS ///////
namespace common {
class UnitCell;
}

namespace build_cell {

class IUnitCellBlueprint
{
public:

  struct StructureInfo
  {
    StructureInfo(const size_t _numAtoms, const double _atomsVolume):
      numAtoms(_numAtoms),
      atomsVolume(_atomsVolume)
      {}
    const size_t numAtoms;
    const double atomsVolume;
  };

  typedef ::boost::optional<const StructureInfo> OptionalStructureInfo;

  virtual ~IUnitCellBlueprint() {}

  virtual common::UnitCellPtr generateCell(const OptionalStructureInfo = OptionalStructureInfo()) const = 0;
};

}
}

#endif /* I_UNIT_CELL_BLUEPRINT_H */
