/*
 * AtomGroupDescription.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#include "AtomGroupDescription.h"

#include <algorithm>

#include "AtomConstraintDescription.h"
#include "AtomsDescription.h"

namespace sstbx { namespace build_cell {

AtomGroupDescription::AtomGroupDescription(AtomGroupDescription * const parent):
myParent(parent)
{}

AtomGroupDescription::~AtomGroupDescription()
{
	// First delete all our child groups
	while(!childGroups.empty())
	{
		delete childGroups.back();
		childGroups.pop_back();
	}
	// Now delete all our atoms
	while(!childAtoms.empty())
	{
		delete childAtoms.back();
		childAtoms.pop_back();
	}
}

void AtomGroupDescription::setParent(AtomGroupDescription * const parent)
{
	myParent = parent;
}

AtomConstraintDescription * AtomGroupDescription::getAtomConstraint(const ConstraintDescriptionId id) const
{
	AtomConstraintDescription * constraint = 0;

	AtomCMap::const_iterator it = myAtomConstraints.find(id);
	if(it != myAtomConstraints.end())
	{
		constraint = it->second;
	}
	else if(myParent)
	{
		// Pass it up the chain
		constraint = myParent->getAtomConstraintInherits(id);
	}

	return constraint;
}

template <class CType>
CType * AtomGroupDescription::getAtomConstraint(const ConstraintDescriptionId id) const
{
	AtomConstraintDescription * constraint = 0;

	AtomCMap::const_iterator it = myAtomConstraints.find(id);
	if(it != myAtomConstraints.end())
	{
		constraint = it->second;
	}
	else if(myParent)
	{
		// Pass it up the chain
		constraint = myParent->getAtomConstraintInherits(id);
	}

	return static_cast<CType>(constraint);
}

void AtomGroupDescription::addAtomConstraint(AtomConstraintDescription * const atomConstraint)
{
	myAtomConstraints.insert(AtomCMapPair(atomConstraint->getType(), atomConstraint));
}

const std::vector<AtomGroupDescription *> & AtomGroupDescription::getChildGroups() const
{
	return childGroups;
}

bool AtomGroupDescription::removeAtomConstraint(AtomConstraintDescription * const atomConstraint)
{
	AtomCMap::iterator it =	myAtomConstraints.find(atomConstraint->getType());

	if(it == myAtomConstraints.end() || (*it).second != atomConstraint) return false;

	myAtomConstraints.erase(it);

	return true;
}


void AtomGroupDescription::addChild(AtomGroupDescription * const group)
{
	// Add this child group to our vector of children
	childGroups.push_back(group);

	// Tell it that we are the new parent
	group->setParent(this);
}

const std::vector<AtomsDescription *> & AtomGroupDescription::getChildAtoms() const
{
	return childAtoms;
}

void AtomGroupDescription::addChild(AtomsDescription * const atoms)
{
	childAtoms.push_back(atoms);

	// Tell it that we are the new parent
	atoms->setParent(this);
}

AtomConstraintDescription * AtomGroupDescription::getAtomConstraintInherits(const ConstraintDescriptionId id) const
{
	AtomConstraintDescription * constraint = 0;

	// Try to find the constraint amongst my constraints
	AtomCMap::const_iterator it = myAtomConstraints.find(id);
	if(it != myAtomConstraints.end() && it->second->inherits())
	{
		constraint = it->second;
	}
	else if(myParent)
	{
		// Pass it up the chain
		constraint = myParent->getAtomConstraintInherits(id);
	}

	return constraint;
}

}}