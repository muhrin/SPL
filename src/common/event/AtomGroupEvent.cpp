/*
 * AtomGroupAtomEvent.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: muhrin
 */

// INCLUDES /////////////////////////////////////
#include "common/event/AtomGroupEvent.h"

namespace sstbx { namespace common { namespace event {

AtomGroupEvent::AtomGroupEvent(
	AtomGroup * source,
	const AtomGroupEvent::EventType & eventType,
	Atom * atom):
mySource(source),
myEventType(eventType),
myAtom(atom)
{}

AtomGroupEvent::AtomGroupEvent(
	AtomGroup * source,
	const AtomGroupEvent::EventType & eventType,
	AtomGroup * group):
mySource(source),
myEventType(eventType),
myGroup(group)
{}

const AtomGroupEvent::EventType & AtomGroupEvent::getEventType() const
{
	return myEventType;
}

AtomGroup * AtomGroupEvent::getSource() const
{
	return mySource;
}

Atom * AtomGroupEvent::getAtom() const
{
	return myAtom;
}

}}}