/*
 * StructureDescriptionVisitor.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_DESCRIPTION_VISITOR_H
#define STRUCTURE_DESCRIPTION_VISITOR_H

// INCLUDES ////////////

namespace sstbx
{
namespace build_cell
{

// FORWARD DECLARATIONS ///////
class AtomGroupDescription;
class AtomsDescription;
class StructureDescription;

class StructureDescriptionVisitor
{
public:
  virtual ~StructureDescriptionVisitor() {};

  virtual void enteringStructure(StructureDescription & structureDescription) {}
  virtual void leavingStructure(StructureDescription & structureDescription) {}
  virtual void enteringAtomGroup(AtomGroupDescription & groupDescription) {}
  virtual void leavingAtomGroup(AtomGroupDescription & groupDescription) {}
  virtual bool visitAtomGroup(AtomGroupDescription & groupDescription) {return true;}
  virtual bool visitAtom(AtomsDescription & atomDescription) {return true;}

protected:
  StructureDescriptionVisitor() {}
};

}
}

#endif /* STRUCTURE_DESCRIPTION_VISITOR_H */
