/*
 * StructureDescription.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#include "build_cell/StructureDescription.h"

#include "build_cell/AtomConstraintDescription.h"
#include "build_cell/AtomsDescription.h"
#include "build_cell/ConstStructureDescriptionVisitor.h"
#include "build_cell/StructureConstraintDescription.h"
#include "build_cell/StructureDescriptionVisitor.h"

namespace sstbx {
namespace build_cell {

StructureDescription::StructureDescription(ConstUnitCellBlueprintPtr unitCell)
{
  myUnitCell = unitCell;
}

void StructureDescription::addStructureConstraint(
  StructureConstraintDescription * const structureConstraint)
{
	myStructureConstraints.push_back(structureConstraint);
}

const StructureDescription::ConstraintsContainer &
StructureDescription::getStructureConstraints()
{
	return myStructureConstraints;
}

bool StructureDescription::traversePreorder(StructureDescriptionVisitor & visitor)
{
  visitor.enteringStructure(*this);

  const bool outcome = AtomGroupDescription::traversePreorder(visitor);

  visitor.leavingStructure(*this);

  return outcome;
}

bool StructureDescription::traversePreorder(ConstStructureDescriptionVisitor & visitor) const
{
  visitor.enteringStructure(*this);

  const bool outcome = AtomGroupDescription::traversePreorder(visitor);

  visitor.leavingStructure(*this);

  return outcome;
}

bool StructureDescription::traversePostorder(StructureDescriptionVisitor & visitor)
{
  visitor.enteringStructure(*this);

  const bool outcome = AtomGroupDescription::traversePostorder(visitor);

  visitor.leavingStructure(*this);

  return outcome;
}

bool StructureDescription::traversePostorder(ConstStructureDescriptionVisitor & visitor) const
{
  visitor.enteringStructure(*this);

  const bool outcome = AtomGroupDescription::traversePostorder(visitor);

  visitor.leavingStructure(*this);

  return outcome;
}

void StructureDescription::setUnitCell(ConstUnitCellBlueprintPtr unitCell)
{
  myUnitCell = unitCell;
}

const IUnitCellBlueprint * StructureDescription::getUnitCell() const
{
  return myUnitCell.get();
}

}
}
