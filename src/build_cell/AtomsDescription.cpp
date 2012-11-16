/*
 * AtomsDescription.h
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */


// INCLUDES /////////////////
#include "build_cell/AtomsDescription.h"

#include "build_cell/AtomConstraintDescription.h"
#include "build_cell/AtomGroupDescription.h"

#include "common/AtomSpeciesId.h"

namespace sstbx {
namespace build_cell {

AtomsDescription::AtomsDescription():
mySpecies(sstbx::common::AtomSpeciesId::CUSTOM_1),
myCount(1),
myParent(NULL)
{
}

AtomsDescription::AtomsDescription(const ::sstbx::common::AtomSpeciesId::Value  species, const size_t count):
mySpecies(species),
myCount(count),
myParent(NULL)
{}

const AtomConstraintDescription *
AtomsDescription::getAtomConstraint(const ConstraintDescriptionId id) const
{
	const AtomConstraintDescription * constraint = 0;

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
	myAtomConstraints.insert(atomConstraint->getType(), atomConstraint);
}

bool AtomsDescription::removeAtomConstraint(const AtomConstraintDescription * const atomConstraint)
{
	AtomCMap::iterator it =	myAtomConstraints.find(atomConstraint->getType());

	if(it == myAtomConstraints.end() || (*it).second != atomConstraint) return false;

	myAtomConstraints.erase(it);

	return true;
}

const ::sstbx::common::AtomSpeciesId::Value & AtomsDescription::getSpecies() const
{
	return mySpecies;
}

void AtomsDescription::setElementType(const ::sstbx::common::AtomSpeciesId::Value  species)
{
	mySpecies = species;
}

size_t AtomsDescription::getCount() const
{
	return myCount;
}

void AtomsDescription::setCount(const size_t newCount)
{
	myCount = newCount;
}

::boost::optional<double> AtomsDescription::getRadius() const
{
  return myRadius;
}

void AtomsDescription::setRadius(const double radius)
{
  myRadius.reset(radius);
}

const AtomGroupDescription * AtomsDescription::getParent() const
{
  return myParent;
}

void AtomsDescription::setParent(const sstbx::build_cell::AtomGroupDescription *const parent)
{
	myParent = parent;
}

}}
