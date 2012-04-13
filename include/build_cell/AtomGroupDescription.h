/*
 * AtomGroupDescription.h
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#ifndef ATOM_GROUP_DESCRIPTION_H
#define ATOM_GROUP_DESCRIPTION_H

// INCLUDES ///////////////////
#include "SSLib.h"
#include "IAtomConstrainable.h"

#include "build_cell/ConstraintDescriptionId.h"

#include <map>
#include <vector>


// FORWARD DECLARES ///////////
namespace sstbx { namespace build_cell {
	class AtomConstraintDescription;
	class AtomsDescription;
	class IConstraintDescription;
}}


namespace sstbx { namespace build_cell {

class AtomGroupDescription : public IAtomConstrainable
{
public:

	friend class AtomsDescription;

	AtomGroupDescription(AtomGroupDescription * const parent = NULL);
	virtual ~AtomGroupDescription();

	/** Child groups */

	const ::std::vector<AtomGroupDescription *> & getChildGroups() const;

	void addChild(AtomGroupDescription * const childGroup);

	/** Child atoms */

	const ::std::vector<AtomsDescription *> & getChildAtoms() const;

	void addChild(AtomsDescription * const childAtom);

	/** IAtomConstrainable */

	virtual AtomConstraintDescription * getAtomConstraint(const ConstraintDescriptionId id) const;

	virtual void addAtomConstraint(AtomConstraintDescription * const atomConstraint);

	virtual bool removeAtomConstraint(AtomConstraintDescription * const atomConstraint);

	template <class CType>
	CType * getAtomConstraint(const ConstraintDescriptionId id) const;

protected:

	typedef ::std::map<ConstraintDescriptionId, AtomConstraintDescription *> AtomCMap;

	typedef ::std::pair<ConstraintDescriptionId, AtomConstraintDescription *> AtomCMapPair;

	/** Parent group */

	void setParent(AtomGroupDescription * const parent);

	AtomConstraintDescription * getAtomConstraintInherits(const ConstraintDescriptionId id) const;

	AtomGroupDescription *					myParent;

	::std::vector<AtomsDescription *>		childAtoms;

	::std::vector<AtomGroupDescription *>	childGroups;

	AtomCMap myAtomConstraints;

};

}}

#endif /* ATOM_GROUP_DESCRIPTION_H */
