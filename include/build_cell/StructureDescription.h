/*
 * StructureDescription.h
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_DESCRIPTION_H
#define STRUCTURE_DESCRIPTION_H

// INCLUDES ///////////////////

#include <boost/ptr_container/ptr_vector.hpp>

#include "build_cell/AtomGroupDescription.h"
#include "build_cell/IUnitCellBlueprint.h"
#include "build_cell/StructureConstraintDescription.h"
#include "build_cell/Types.h"

// FORWARD DECLARES ///////////

namespace sstbx {
namespace build_cell {

class StructureDescription : public AtomGroupDescription
{
public:
  typedef boost::ptr_vector<StructureConstraintDescription> ConstraintsContainer;

  StructureDescription() {}
  explicit StructureDescription(ConstUnitCellBlueprintPtr unitCell);

	void addStructureConstraint(StructureConstraintDescription * const structureConstraint);	

	ConstraintsContainer const & getStructureConstraints();

  // From AtomGroupDescription /////////////////////
  // Visit each atom group first and then child groups
  virtual bool traversePreorder(StructureDescriptionVisitor & visitor);
  virtual bool traversePreorder(ConstStructureDescriptionVisitor & visitor) const;
  // Visit child groups before visiting this group
  virtual bool traversePostorder(StructureDescriptionVisitor & visitor);
  virtual bool traversePostorder(ConstStructureDescriptionVisitor & visitor) const;
  // End from AtomGroupDescription ///////////////

  void setUnitCell(ConstUnitCellBlueprintPtr unitCell);
  const IUnitCellBlueprint * getUnitCell() const;

protected:

  ConstraintsContainer      myStructureConstraints;
  ConstUnitCellBlueprintPtr myUnitCell;

};

}
}

#endif /* STRUCTURE_DESCRIPTION_H */
