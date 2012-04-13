/*
 * StructureBuilder.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#include "StructureBuilder.h"

#include "AtomsDescription.h"
#include "build_cell/AtomGroupDescription.h"
#include "Minsep.h"
#include "StructureDescription.h"

#include "common/Atom.h"
#include "../common/AtomGroup.h"
#include "common/Structure.h"


namespace sstbx { namespace build_cell {

	StructureBuilder::StructureBuilder(const StructureDescription * structureDescription, ::sstbx::common::Structure * const structure):
myStructureDescription(structureDescription),
myStructure(structure)
{
	// Build the structure from the description
	buildAtomGroup(structureDescription, *myStructure);
}

::sstbx::common::Structure * StructureBuilder::getStructure() const
{
	return myStructure;
}

const AtomsDescription * StructureBuilder::getAtomsDescription(
	const ::sstbx::common::Atom * const atom) const
{
	this->myAtomsMap.find(atom);
	AtomMap::const_iterator it = myAtomsMap.find(atom);

	const AtomsDescription * desc = NULL;
	
	if(it != myAtomsMap.end())
	{
		desc = it->second;
	}

	return desc;
}

const AtomGroupDescription * StructureBuilder::getAtomGroupDescription(
	const ::sstbx::common::AtomGroup * const group) const
{
	AtomGroupMap::const_iterator it = myAtomGroupsMap.find(group);

	const AtomGroupDescription * desc = NULL;
	
	if(it != myAtomGroupsMap.end())
	{
		desc = it->second;
	}

	return desc;
}

void StructureBuilder::buildAtomGroup(
	const AtomGroupDescription * groupDescription,
	::sstbx::common::AtomGroup & group)
{
	using ::std::vector;
	using ::sstbx::common::Atom;
	using ::sstbx::common::AtomGroup;

	// First create the atoms
	const vector<AtomsDescription *> & atoms = groupDescription->getChildAtoms();
	for(vector<AtomsDescription *>::const_iterator it = atoms.begin(), end = atoms.end();
		it != end; ++it)
	{
		const AtomsDescription * desc = *it;
		for(size_t i = 0; i < desc->getCount(); ++i)
		{
			Atom * const a = new Atom(desc->getSpecies());
			group.insertAtom(a);
			// Add it to the map
			myAtomsMap[a] = desc;
		}
	}

	// And now any child groups
	const vector<AtomGroupDescription *> & groups = groupDescription->getChildGroups();
	for(vector<AtomGroupDescription *>::const_iterator it = groups.begin(), end = groups.end();
		it != end; ++it)
	{
		AtomGroupDescription * desc = *it;
		AtomGroup * childGroup = new AtomGroup();
		group.insertGroup(childGroup);
		// Add it to the map
		myAtomGroupsMap[childGroup] =  desc;
		buildAtomGroup(desc, *childGroup);
	}
}

}}