/*
 * ConstVisitorGroup.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */


// INCLUDES //////////////////////////////////////
#include "build_cell/ConstVisitorGroup.h"

#include <boost/foreach.hpp>


namespace sstbx
{
namespace build_cell
{

void ConstVisitorGroup::pushBack(ConstStructureDescriptionVisitor & visitor)
{
  myVisitors.push_back(&visitor);
}

void ConstVisitorGroup::enteringStructure(const StructureDescription & structureDescription)
{
  ConstStructureDescriptionVisitor * visitor;
  BOOST_FOREACH(visitor, myVisitors)
  {
    visitor->enteringStructure(structureDescription);
  }
}

void ConstVisitorGroup::leavingStructure(const StructureDescription & structureDescription)
{
  ConstStructureDescriptionVisitor * visitor;
  BOOST_FOREACH(visitor, myVisitors)
  {
    visitor->leavingStructure(structureDescription);
  }
}

void ConstVisitorGroup::enteringAtomGroup(const AtomGroupDescription & atomGroup)
{
  ConstStructureDescriptionVisitor * visitor;
  BOOST_FOREACH(visitor, myVisitors)
  {
    visitor->enteringAtomGroup(atomGroup);
  }
}

void ConstVisitorGroup::leavingAtomGroup(const AtomGroupDescription & atomGroup) 
{
  ConstStructureDescriptionVisitor * visitor;
  BOOST_FOREACH(visitor, myVisitors)
  {
    visitor->leavingAtomGroup(atomGroup);
  }
}

bool ConstVisitorGroup::visitAtomGroup(const AtomGroupDescription & atomGroup)
{
  bool continueVisit = true;
  ConstStructureDescriptionVisitor * visitor;
  BOOST_FOREACH(visitor, myVisitors)
  {
    continueVisit &= visitor->visitAtomGroup(atomGroup);
  }
  return continueVisit;
}

bool ConstVisitorGroup::visitAtom(const AtomsDescription & atom)
{
  bool continueVisit = true;
  ConstStructureDescriptionVisitor * visitor;
  BOOST_FOREACH(visitor, myVisitors)
  {
    continueVisit &= visitor->visitAtom(atom);
  }
  return continueVisit;
}


}
}
