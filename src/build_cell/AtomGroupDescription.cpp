/*
 * AtomGroupDescription.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#include "build_cell/AtomGroupDescription.h"

#include <algorithm>
#include <limits>

#include <boost/foreach.hpp>

// Local includes
#include "SSLibAssert.h"
#include "build_cell/AtomConstraintDescription.h"
#include "build_cell/AtomsDescription.h"
#include "build_cell/ConstStructureDescriptionVisitor.h"
#include "build_cell/StructureDescriptionVisitor.h"

namespace sstbx
{
namespace build_cell
{

const unsigned int AtomGroupDescription::DEPTH_ALL_CHILD_GROUPS = ::std::numeric_limits<unsigned int>::max();

AtomGroupDescription::AtomGroupDescription(AtomGroupDescription * const parent):
myParent(parent)
{}

AtomGroupDescription::~AtomGroupDescription()
{}

void AtomGroupDescription::setParent(AtomGroupDescription * const parent)
{
	myParent = parent;
}

const AtomConstraintDescription *
AtomGroupDescription::getAtomConstraint(const ConstraintDescriptionId id) const
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

bool AtomGroupDescription::traversePreorder(StructureDescriptionVisitor & visitor)
{
  visitor.enteringAtomGroup(*this);

  // Visit this node
  bool continueVisiting = visitor.visitAtomGroup(*this);

  if(continueVisiting)
  {

    // Then visit our atoms
    BOOST_FOREACH(AtomsDescription & atom, myAtoms)
    {
      if(!visitor.visitAtom(atom))
      {
        continueVisiting = false;
        break;
      }
    }

    if(continueVisiting)
    {
      // Then visit child groups
      BOOST_FOREACH(AtomGroupDescription & group, myGroups)
      {
        group.traversePreorder(visitor);
      }
    }
  }

  visitor.leavingAtomGroup(*this);

  return continueVisiting;
}

bool AtomGroupDescription::traversePreorder(ConstStructureDescriptionVisitor & visitor) const
{
  visitor.enteringAtomGroup(*this);

  // Visit this node
  bool continueVisiting = visitor.visitAtomGroup(*this);

  if(continueVisiting)
  {

    // Then visit our atoms
    BOOST_FOREACH(const AtomsDescription & atom, myAtoms)
    {
      if(!visitor.visitAtom(atom))
      {
        continueVisiting = false;
        break;
      }
    }

    if(continueVisiting)
    {
      // Then visit child groups
      BOOST_FOREACH(const AtomGroupDescription & group, myGroups)
      {
        group.traversePreorder(visitor);
      }
    }
  }

  visitor.leavingAtomGroup(*this);

  return continueVisiting;
}

bool AtomGroupDescription::traversePostorder(StructureDescriptionVisitor & visitor)
{
  visitor.enteringAtomGroup(*this);

  bool continueVisiting = true;

  // Visit child groups fist
  BOOST_FOREACH(AtomGroupDescription & group, myGroups)
  {
    if(!group.traversePreorder(visitor))
    {
      continueVisiting = false;
      break;
    }
  }

  if(continueVisiting)
  {
    // Then our atoms
    BOOST_FOREACH(AtomsDescription & atom, myAtoms)
    {
      if(!visitor.visitAtom(atom))
      {
        continueVisiting = false;
        break;
      }
    }

    if(continueVisiting)
    {
      // Then this child group
      continueVisiting = visitor.visitAtomGroup(*this);
    }
  }

  visitor.leavingAtomGroup(*this);

  return continueVisiting;
}

bool AtomGroupDescription::traversePostorder(ConstStructureDescriptionVisitor & visitor) const
{
  visitor.enteringAtomGroup(*this);

  bool continueVisiting = true;

  // Visit child groups fist
  BOOST_FOREACH(const AtomGroupDescription & group, myGroups)
  {
    if(!group.traversePreorder(visitor))
    {
      continueVisiting = false;
      break;
    }
  }

  if(continueVisiting)
  {
    // Then our atoms
    BOOST_FOREACH(const AtomsDescription & atom, myAtoms)
    {
      if(!visitor.visitAtom(atom))
      {
        continueVisiting = false;
        break;
      }
    }

    if(continueVisiting)
    {
      // Then this child group
      continueVisiting = visitor.visitAtomGroup(*this);
    }
  }

  visitor.leavingAtomGroup(*this);

  return continueVisiting;
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
  SSLIB_ASSERT(atomConstraint);

  myAtomConstraints.insert(atomConstraint->getType(), atomConstraint);
}

const AtomGroupDescription::GroupsContainer &
AtomGroupDescription::getChildGroups() const
{
	return myGroups;
}

bool AtomGroupDescription::removeAtomConstraint(const AtomConstraintDescription * const atomConstraint)
{
	AtomCMap::iterator it =	myAtomConstraints.find(atomConstraint->getType());

	if(it == myAtomConstraints.end() || (*it).second != atomConstraint) return false;

	myAtomConstraints.erase(it);

	return true;
}


void AtomGroupDescription::addChild(AtomGroupDescription * const group)
{
	// Add this child group to our vector of children
	myGroups.push_back(group);

	// Tell it that we are the new parent
	group->setParent(this);
}

void AtomGroupDescription::clearChildGroups()
{
  myGroups.clear();
}

const AtomGroupDescription::AtomsContainer &
AtomGroupDescription::getChildAtoms() const
{
	return myAtoms;
}

void AtomGroupDescription::addChild(AtomsDescriptionPtr atoms)
{
  // Insert and tell it that we are the new parent
  myAtoms.insert(myAtoms.end(), atoms.release())->setParent(this);
}

void AtomGroupDescription::clearChildAtoms()
{
  myAtoms.clear();
}

void AtomGroupDescription::getAtomSpecies(::std::set<common::AtomSpeciesId::Value> & species, const unsigned int maxDepth) const
{
  BOOST_FOREACH(const AtomsDescription & atom, myAtoms)
  {
    species.insert(atom.getSpecies());
  }

  if(maxDepth > 0)
  {
    BOOST_FOREACH(const AtomGroupDescription & childGroup, myGroups)
    {
      childGroup.getAtomSpecies(species, maxDepth - 1);
    }
  }
}

void AtomGroupDescription::setAtomRadii(const double radius)
{
  // Set for all atoms
  BOOST_FOREACH(AtomsDescription & atomDescription, myAtoms)
  {
    atomDescription.setRadius(radius);
  }
}

void AtomGroupDescription::setAtomRadiiDescend(const double radius)
{
  // Set for all of my atoms
  setAtomRadii(radius);

  // Set for all child groups
  BOOST_FOREACH(AtomGroupDescription & groupDescription, myGroups)
  {
    groupDescription.setAtomRadiiDescend(radius);
  }
}

const AtomConstraintDescription *
AtomGroupDescription::getAtomConstraintInherits(const ConstraintDescriptionId id) const
{
	const AtomConstraintDescription * constraint = 0;

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

}
}
