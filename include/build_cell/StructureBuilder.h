/*
 * StructureBuilder.h
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_BUILDER_H
#define STRUCTURE_BUILDER_H

// INCLUDES ///////////////////
#include <map>

// FORWARD DECLARES ///////////
namespace sstbx
{
	namespace build_cell
	{
		class AtomsDescription;
		class AtomGroupDescription;
		class StructureDescription;
	}
	namespace common
	{
		class Atom;
		class AtomGroup;
		class Structure;
	}
}

namespace sstbx { namespace build_cell {

class StructureBuilder
{
public:

	StructureBuilder(
		const StructureDescription * structureDescription,
		::sstbx::common::Structure * const structure);

	::sstbx::common::Structure * getStructure() const;

	const AtomsDescription * getAtomsDescription(const ::sstbx::common::Atom * const atom) const;

	const AtomGroupDescription * getAtomGroupDescription(const ::sstbx::common::AtomGroup * const group) const;


private:

	typedef ::std::map<const ::sstbx::common::Atom *, const AtomsDescription *> AtomMap;

	typedef ::std::pair<const ::sstbx::common::Atom *, const AtomsDescription *> AtomMapPair;

	typedef ::std::map<const ::sstbx::common::AtomGroup *, const AtomGroupDescription *> AtomGroupMap;

	typedef ::std::pair<const ::sstbx::common::AtomGroup *, const AtomGroupDescription *> AtomGroupMapPair;

	void buildAtomGroup(const AtomGroupDescription * groupDescription, ::sstbx::common::AtomGroup & group);

	const StructureDescription * myStructureDescription;

	::sstbx::common::Structure * myStructure;

	AtomMap			myAtomsMap;

	AtomGroupMap	myAtomGroupsMap;

};

}}

#endif /* STRUCTURE_BUILDER_H */
