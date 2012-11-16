/*
 * VisitorGroup.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */


// INCLUDES //////////////////////////////////////
#include "build_cell/VisitorGroup.h"

#include <boost/foreach.hpp>


namespace sstbx
{
namespace build_cell
{

void VisitorGroup::pushBack(StructureDescriptionVisitor & visitor)
{
  myVisitors.push_back(&visitor);
}

void VisitorGroup::enteringStructure(StructureDescription & structureDescription)
{
  StructureDescriptionVisitor * visitor;
  BOOST_FOREACH(visitor, myVisitors)
  {
    visitor->enteringStructure(structureDescription);
  }
}

void VisitorGroup::leavingStructure(StructureDescription & structureDescription)
{
  StructureDescriptionVisitor * visitor;
  BOOST_FOREACH(visitor, myVisitors)
  {
    visitor->leavingStructure(structureDescription);
  }
}

void VisitorGroup::enteringAtomGroup(AtomGroupDescription & atomGroup)
{
  StructureDescriptionVisitor * visitor;
  BOOST_FOREACH(visitor, myVisitors)
  {
    visitor->enteringAtomGroup(atomGroup);
  }
}

void VisitorGroup::leavingAtomGroup(AtomGroupDescription & atomGroup) 
{
  StructureDescriptionVisitor * visitor;
  BOOST_FOREACH(visitor, myVisitors)
  {
    visitor->leavingAtomGroup(atomGroup);
  }
}

bool VisitorGroup::visitAtomGroup(AtomGroupDescription & atomGroup)
{
  bool continueVisit = true;
  StructureDescriptionVisitor * visitor;
  BOOST_FOREACH(visitor, myVisitors)
  {
    continueVisit &= visitor->visitAtomGroup(atomGroup);
  }
  return continueVisit;
}

bool VisitorGroup::visitAtom(AtomsDescription & atom)
{
  bool continueVisit = true;
  StructureDescriptionVisitor * visitor;
  BOOST_FOREACH(visitor, myVisitors)
  {
    continueVisit &= visitor->visitAtom(atom);
  }
  return continueVisit;
}


}
}
