/*
 * StructureListener.h
 *
 *  Created on: Sep 29, 2013
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_LISTENER_H
#define STRUCTURE_LISTENER_H

// INCLUDES ///////////////////////////////////////////////
#include "spl/SSLib.h"

namespace spl {
// FORWARD DECLARATIONS ////////////////////////////////////
namespace common {
class Structure;
class UnitCell;

class StructureListener
{
public:
  virtual
  ~StructureListener()
  {
  }
  virtual void
  onUnitCellChanged(Structure * const structure, UnitCell * const oldCell,
      UnitCell * const newCell) = 0;
  virtual void
  onStructureDestroyed(const Structure & structure) = 0;
};

}
}

#endif /* STRUCTURE_LISTENER_H */
