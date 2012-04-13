/*
 * AtomGroup.cpp
 *
 *  Created on: Nov 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES /////////////////
#include "SSLib.h"

#include "common/Atom.h"
#include "common/AtomGroup.h"
#include "StructureTreeEvent.h"

#include <set>


namespace sstbx { namespace common {

AtomGroup::AtomGroup():
myParent(NULL)
{}

AtomGroup::~AtomGroup()
{
	using ::std::vector;

	for(vector<Atom *>::const_iterator it = atoms.begin(), end = atoms.end();
		it != end; ++it)
	{
		delete (*it);
	}
	atoms.clear();
}

const AtomGroup::Vec3 & AtomGroup::getPosition() const
{
	return position;
}

void AtomGroup::setPosition(const AtomGroup::Vec3 & pos)
{
	position = pos;
}

void AtomGroup::getAtomPositions(Mat & posMtx, const size_t startCol) const
{
	const size_t numAtoms = atoms.size();

	// Make sure the matrix of the right size
	if(posMtx.n_rows != 3 || posMtx.n_cols < numAtoms)
		posMtx.set_size(3, startCol + numAtoms);
	
	for(size_t i = startCol; i < startCol + numAtoms; ++i)
	{
		posMtx.col(i) = atoms[i]->getPosition();
	}
}

void AtomGroup::setAtomPositions(const Mat & posMtx, const size_t startCol)
{
	const size_t numAtoms = atoms.size();

	// Make sure the matrix has the correct proportions
	SSE_ASSERT(posMtx.n_rows == 3 && posMtx.n_cols >= (numAtoms + startCol));

	for(size_t i = startCol; i < numAtoms + startCol; ++i)
	{
		atoms[i]->setPosition(posMtx.col(i));
	}
}

void AtomGroup::getAtomPositionsDescendent(Mat & posMtx, const size_t startCol) const
{
	using namespace std;

	const size_t numAtoms		= getNumAtomsDescendent();
	// Make sure the matrix is big enough
	if(posMtx.n_rows != 3 || posMtx.n_cols < startCol + numAtoms)
		posMtx.set_size(3, startCol + numAtoms);

	// First get my atoms
	const size_t numGroupAtoms	= getNumAtoms();
	getAtomPositions(posMtx, startCol);

	size_t currentCol = startCol + numGroupAtoms;
	// Now all the atoms of my child groups
	for(vector<AtomGroup *>::const_iterator it = groups.begin(), end = groups.end();
		it != end; ++it)
	{
		const AtomGroup * const childGroup = *it;
		const size_t numChildGroupAtoms = childGroup->getNumAtomsDescendent();

		childGroup->getAtomPositionsDescendent(posMtx, currentCol);

		// Now displace them by the current group position
		const AtomGroup::Vec3 groupPos = childGroup->getPosition();
		for(size_t i = currentCol; i < currentCol + numChildGroupAtoms; ++i)
		{
			posMtx.col(i) += groupPos;
		}
		currentCol += numChildGroupAtoms;
	}
}

void AtomGroup::setAtomPositionsDescendent(const Mat & posMtx, const size_t startCol)
{
	using ::std::vector;

	const size_t numPositions	= posMtx.n_cols - startCol;
	const size_t myNumAtoms		= atoms.size();

	// First set my atoms
	setAtomPositions(posMtx, startCol);

	// Now all the atoms of my child groups
	size_t currentPos = myNumAtoms + startCol;
	for(vector<AtomGroup *>::const_iterator it = groups.begin(), end = groups.end();
		it != end; ++it)
	{
		AtomGroup * const childGroup = *it;
		const size_t childNumAtoms = childGroup->getNumAtomsDescendent();

		Mat childPositions = posMtx.cols(currentPos, currentPos + childNumAtoms - 1);

		// Now displace them by the current group position
		const Vec3 groupPos = childGroup->getPosition();
		for(size_t i = 0; i < childPositions.n_cols; ++i)
		{
			childPositions.col(i) -= groupPos;
		}

		// Finally set the positions
		childGroup->setAtomPositionsDescendent(childPositions, currentPos);

		currentPos += childNumAtoms;
	}

}

size_t AtomGroup::getNumAtoms() const
{
	return atoms.size();
}

size_t AtomGroup::getNumAtomsDescendent() const
{
	using namespace std;

	size_t totalAtoms = getNumAtoms();

	for(vector<AtomGroup *>::const_iterator it = groups.begin(), end = groups.end();
		it != end; ++it)
	{
		totalAtoms += (*it)->getNumAtoms();
	}

	return totalAtoms;
}

size_t AtomGroup::getNumAtomSpecies() const
{
	::std::set<AtomSpeciesId> speciesSet;
	for(size_t i = 0; i < atoms.size(); ++i)
	{
		speciesSet.insert(atoms[i]->getSpecies());
	}
	return speciesSet.size();
}

size_t AtomGroup::getNumAtomSpeciesDescendent() const
{
	::std::vector<AtomSpeciesId> species;
	getAtomSpeciesDescendent(species);

	::std::set<AtomSpeciesId> speciesSet;
	speciesSet.insert(species.begin(), species.end());

	return speciesSet.size();
}


void AtomGroup::getAtomSpecies(::std::vector<AtomSpeciesId> & species) const
{
	for(size_t i = 0; i < atoms.size(); ++i)
	{
		species.push_back(atoms[i]->getSpecies());
	}
}

void AtomGroup::getAtomSpeciesDescendent(::std::vector<AtomSpeciesId> & species) const
{
	// First do mine...
	getAtomSpecies(species);

	// Now all the child groups
	for(size_t i = 0; i < groups.size(); ++i)
	{
		getAtomSpeciesDescendent(species);
	}
}

const std::vector<Atom *> & AtomGroup::getAtoms() const
{
	return atoms;
}

void AtomGroup::insertAtom(Atom * const atom)
{
	atoms.push_back(atom);

	atom->setParent(this);

	StructureTreeEvent evt(StructureTreeEvent::ATOM_INSERTED, *atom);
	eventFired(evt);
}

bool AtomGroup::removeAtom(Atom * const atom)
{
	::std::vector<Atom *>::iterator it = ::std::find(atoms.begin(), atoms.end(), atom);

	if(it == atoms.end()) return false;

	atom->setParent(NULL);
	atoms.erase(it);

	StructureTreeEvent evt(StructureTreeEvent::ATOM_REMOVED, *atom);
	eventFired(evt);

	return true;
}

const std::vector<AtomGroup *> & AtomGroup::getGroups() const
{
	return groups;
}

void AtomGroup::insertGroup(AtomGroup * const childGroup)
{
	groups.push_back(childGroup);

	childGroup->setParent(this);

	StructureTreeEvent evt(StructureTreeEvent::GROUP_INSERTED, *childGroup);
}

bool AtomGroup::removeGroup(AtomGroup * const childGroup)
{
	using namespace std;
	vector<AtomGroup *>::iterator it = find(groups.begin(), groups.end(), childGroup);

	// If we couldn't find the child then return false
	if(it == groups.end()) return false;

	childGroup->setParent(NULL);
	groups.erase(it);

	StructureTreeEvent evt(StructureTreeEvent::GROUP_REMOVED, *childGroup);
	eventFired(evt);

	return true;
}

void AtomGroup::setParent(sstbx::common::AtomGroup *const parent)
{
	myParent = parent;
}

void AtomGroup::eventFired(const StructureTreeEvent & evt)
{
	// Propagate this to my parent
	if(myParent)
	{
		myParent->eventFired(evt);
	}
}

}}