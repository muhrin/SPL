/*
 * AtomsDescription.h
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */


// INCLUDES /////////////////
#include "AtomsDescription.h"

#include "AtomConstraintDescription.h"
#include "AtomGroupDescription.h"

#include "../common/AtomSpeciesId.h"

namespace sstbx { namespace build_cell {

AtomsDescription::AtomsDescription():
mySpecies(sstbx::common::CUSTOM_1),
count(0)
{
}

AtomsDescription::AtomsDescription(const ::sstbx::common::AtomSpeciesId species, const size_t count):
mySpecies(species),
count(count)
{}

AtomConstraintDescription * AtomsDescription::getAtomConstraint(const ConstraintDescriptionId id) const
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

void AtomsDescription::addAtomConstraint(AtomConstraintDescription * const atomConstraint)
{
	myAtomConstraints.insert(AtomCMapPair(atomConstraint->getType(), atomConstraint));
}

bool AtomsDescription::removeAtomConstraint(AtomConstraintDescription * const atomConstraint)
{
	AtomCMap::iterator it =	myAtomConstraints.find(atomConstraint->getType());

	if(it == myAtomConstraints.end() || (*it).second != atomConstraint) return false;

	myAtomConstraints.erase(it);

	return true;
}

const ::sstbx::common::AtomSpeciesId AtomsDescription::getSpecies() const
{
	return mySpecies;
}

void AtomsDescription::setElementType(const ::sstbx::common::AtomSpeciesId species)
{
	mySpecies = species;
}

size_t AtomsDescription::getCount() const
{
	return count;
}

void AtomsDescription::setCount(const size_t newCount)
{
	count = newCount;
}

void AtomsDescription::setParent(const sstbx::build_cell::AtomGroupDescription *const parent)
{
	myParent = parent;
}

}}