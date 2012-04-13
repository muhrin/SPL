/*
 * StructureTreeEvent.h
 *
 *  Created on: Nov 18, 2011
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_TREE_EVENT_H
#define STRUCTURE_TREE_EVENT_H

// INCLUDES ///////////////////////////////////
#include <vector>

#include <armadillo>

#include "common/event/AtomGroupEvent.h"
#include "common/event/GenericEventSupport.h"
#include "common/event/IGenericEventListener.h"

// FORWARD DECLARES ///////////////////////////
namespace sstbx { namespace common {
	class Atom;
	class AtomGroup;
}}

namespace sstbx { namespace common {

class StructureTreeEvent
{
public:

	enum EventType {
		GROUP_INSERTED,
		GROUP_REMOVED,
		ATOM_INSERTED,
		ATOM_CHANGED,
		ATOM_REMOVED
	};

	StructureTreeEvent(
		const EventType eventType,
		Atom & atom);

	StructureTreeEvent(
		const EventType eventType,
		AtomGroup & group);

	const EventType getEventType() const;
	Atom & getAtom() const;
	AtomGroup & getGroup() const;

private:
	const EventType		myEventType;
	AtomGroup *	const	myGroup;
	Atom * const		myAtom;

};

}}

#endif /* STRUCTURE_TREE_EVENT_H */
