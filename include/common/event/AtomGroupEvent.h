/*
 * AtomGroupEvent.h
 *
 *  Created on: Nov 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef ATOM_GROUP_EVENT_H
#define ATOM_GROUP_EVENT_H

// INCLUDES ///////////////////////////////////

// Include so we can typedef custom event types
#include "GenericEventSupport.h"
#include "IGenericEventListener.h"

// FORWARD DECLARES ///////////////////////////
namespace sstbx { namespace common {
	class Atom;
	class AtomGroup;
}}

// TYPEDEFS ////////////////////////////////////

namespace sstbx { namespace common { namespace event {

class AtomGroupEvent
{
public:
	enum EventType {
		GROUP_INSERTED,
		GROUP_REMOVED,
		ATOM_INSERTED,
		ATOM_REMOVED
	};

	AtomGroupEvent(
		AtomGroup * source,
		const EventType & eventType,
		Atom * atom);

	AtomGroupEvent(
		AtomGroup * source,
		const EventType & eventType,
		AtomGroup * group);

	const EventType & getEventType() const;
	AtomGroup * getSource() const;
	Atom * getAtom() const;
	AtomGroup * getGroup() const;

private:
	EventType 	myEventType;
	AtomGroup * mySource;
	AtomGroup * myGroup;
	Atom *		myAtom;
};


// Convenience typedef for listeners of this event
typedef IGenericEventListener<AtomGroupEvent> AtomGroupListener;
typedef GenericEventSupport<AtomGroupEvent> AtomGroupEventSupport;

}}}

#endif /* ATOM_GROUP_EVENT_H */
