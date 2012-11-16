/*
 * ConstVisitorGroup.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef CONST_VISITOR_GROUP_H
#define CONST_VISITOR_GROUP_H

// INCLUDES ////////////
#include "build_cell/ConstStructureDescriptionVisitor.h"

#include <vector>

namespace sstbx
{
namespace build_cell
{


class ConstVisitorGroup : public ConstStructureDescriptionVisitor
{
public:

  void pushBack(ConstStructureDescriptionVisitor & visitor);

  // From StructureDescriptionVisitor ///////////
  virtual void enteringStructure(const StructureDescription & structureDescription);
  virtual void leavingStructure(const StructureDescription & structureDescription);
  virtual void enteringAtomGroup(const AtomGroupDescription & atomGroup);
  virtual void leavingAtomGroup(const AtomGroupDescription & atomGroup);
  virtual bool visitAtomGroup(const AtomGroupDescription & atomGroup);
  virtual bool visitAtom(const AtomsDescription & atom);
  // End from StructureDescriptionVisitor //////
private:

  ::std::vector<ConstStructureDescriptionVisitor *>  myVisitors;
};

}
}

#endif /* CONST_VISITOR_GROUP_H */
