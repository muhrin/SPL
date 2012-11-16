/*
 * VisitorGroup.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef VISITOR_GROUP_H
#define VISITOR_GROUP_H

// INCLUDES ////////////
#include "build_cell/StructureDescriptionVisitor.h"

#include <vector>

namespace sstbx
{
namespace build_cell
{


class VisitorGroup : public StructureDescriptionVisitor
{
public:

  void pushBack(StructureDescriptionVisitor & visitor);


  // From StructureDescriptionVisitor ///////////
  virtual void enteringStructure(StructureDescription & structureDescription);
  virtual void leavingStructure(StructureDescription & structureDescription);
  virtual void enteringAtomGroup(AtomGroupDescription & atomGroup);
  virtual void leavingAtomGroup(AtomGroupDescription & atomGroup);
  virtual bool visitAtomGroup(AtomGroupDescription & atomGroup);
  virtual bool visitAtom(AtomsDescription & atom);
  // End from StructureDescriptionVisitor //////
private:

  ::std::vector<StructureDescriptionVisitor *>  myVisitors;
};

}
}

#endif /* VISITOR_GROUP_H */
