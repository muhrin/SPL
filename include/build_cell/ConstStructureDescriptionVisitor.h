/*
 * ConstStructureDescriptionVisitor.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef CONST_STRUCTURE_DESCRIPTION_VISITOR_H
#define CONST_STRUCTURE_DESCRIPTION_VISITOR_H

// INCLUDES ////////////
#include "build_cell/StructureDescriptionVisitor.h"

namespace sstbx
{
namespace build_cell
{

// FORWARD DECLARATIONS ///////
class AtomGroupDescription;
class AtomsDescription;
class StructureDescription;

class ConstStructureDescriptionVisitor : public StructureDescriptionVisitor
{
public:
  virtual ~ConstStructureDescriptionVisitor() {};

  virtual void enteringStructure(const StructureDescription & structureDescription) {}
  virtual void leavingStructure(const StructureDescription & structureDescription) {}
  virtual void enteringAtomGroup(const AtomGroupDescription & groupDescription) {}
  virtual void leavingAtomGroup(const AtomGroupDescription & groupDescription) {}
  virtual bool visitAtomGroup(const AtomGroupDescription & groupDescription) {return true;}
  virtual bool visitAtom(const AtomsDescription & atomDescription) {return true;}

protected:
  ConstStructureDescriptionVisitor() {}
};

}
}

#endif /* CONST_STRUCTURE_DESCRIPTION_VISITOR_H */
