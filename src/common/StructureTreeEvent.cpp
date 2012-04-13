/*
 * StructureTreeEvent.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: muhrin
 */

// INCLUDES /////////////////////////////////////
#include "StructureTreeEvent.h"

#include "Atom.h"
#include "AtomGroup.h"

namespace sstbx { namespace common {

StructureTreeEvent::StructureTreeEvent(
	const EventType eventType,
	Atom & atom):
myEventType(eventType),
myAtom(&atom),
myGroup(NULL)
{}

StructureTreeEvent::StructureTreeEvent(
	const EventType eventType,
	AtomGroup & group):
myEventType(eventType),
myGroup(&group),
myAtom(NULL)
{}

const StructureTreeEvent::EventType StructureTreeEvent::getEventType() const
{
	return myEventType;
}

Atom & StructureTreeEvent::getAtom() const
{
	return *myAtom;
}

AtomGroup & StructureTreeEvent::getGroup() const
{
	return *myGroup;
}

}}