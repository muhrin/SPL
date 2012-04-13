/*
 * Atom.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES ///////////////
#include "common/Atom.h"

#include "common/AtomGroup.h"
#include "common/StructureTreeEvent.h"

namespace sstbx { namespace common {

Atom::Atom(const AtomSpeciesId species):
mySpecies(species),
myParent(NULL)
{}

const Atom::Vec3 & Atom::getPosition() const
{
	return position;
}

void Atom::setPosition(const Atom::Vec3 & pos)
{
	position = pos;

	StructureTreeEvent evt(StructureTreeEvent::ATOM_CHANGED, *this);
	eventFired(evt);
}

const AtomSpeciesId Atom::getSpecies() const
{
	return mySpecies;
}

AtomGroup * Atom::getParent() const
{
	return myParent;
}

void Atom::setParent(sstbx::common::AtomGroup *const parent)
{
	myParent = parent;
}

void Atom::eventFired(const StructureTreeEvent & evt)
{
	// Propagate this to my parent
	if(myParent)
	{
		myParent->eventFired(evt);
	}
}

}}