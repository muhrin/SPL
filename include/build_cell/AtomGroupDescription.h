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

#include <set>

#include <boost/ptr_container/ptr_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "build_cell/AtomConstraintDescription.h"
#include "build_cell/ConstraintDescriptionId.h"
#include "build_cell/IAtomConstrainable.h"
#include "build_cell/Types.h"
#include "common/AtomSpeciesId.h"



namespace sstbx {
namespace build_cell {

// FORWARD DECLARES ///////////

class AtomsDescription;
class ConstStructureDescriptionVisitor;
class IConstraintDescription;
class StructureDescriptionVisitor;

class AtomGroupDescription : public IAtomConstrainable
{
public:

  typedef ::boost::ptr_vector<AtomsDescription>     AtomsContainer;
  typedef ::boost::ptr_vector<AtomGroupDescription> GroupsContainer;

	friend class AtomsDescription;

  static const unsigned int DEPTH_LOCAL_GROUP_ONLY = 0;
  static const unsigned int DEPTH_ALL_CHILD_GROUPS;

	AtomGroupDescription(AtomGroupDescription * const parent = NULL);
	virtual ~AtomGroupDescription();

	/** Child groups */

	const GroupsContainer & getChildGroups() const;

	void addChild(AtomGroupDescription * const childGroup);

  void clearChildGroups();

	/** Child atoms */

	const AtomsContainer & getChildAtoms() const;

	void addChild(AtomsDescriptionPtr childAtom);

  void clearChildAtoms();

  void getAtomSpecies(::std::set<common::AtomSpeciesId::Value> & species, const unsigned int maxDepth) const;

  // Atom radii /////////////////////
  void setAtomRadii(const double radius);
  void setAtomRadiiDescend(const double radius);

	// From IAtomConstrainable //
	virtual const AtomConstraintDescription * getAtomConstraint(const ConstraintDescriptionId id) const;
	virtual void addAtomConstraint(AtomConstraintDescription * const atomConstraint);
	virtual bool removeAtomConstraint(const AtomConstraintDescription * const atomConstraint);
  // End from IAtomConstrainable

	template <class CType>
	CType * getAtomConstraint(const ConstraintDescriptionId id) const;

  // Visit each atom group first and then child groups
  virtual bool traversePreorder(StructureDescriptionVisitor & visitor);
  virtual bool traversePreorder(ConstStructureDescriptionVisitor & visitor) const;
  // Visit child groups before visiting this group
  virtual bool traversePostorder(StructureDescriptionVisitor & visitor);
  virtual bool traversePostorder(ConstStructureDescriptionVisitor & visitor) const;

protected:

  typedef ::boost::ptr_map<const ConstraintDescriptionId, AtomConstraintDescription> AtomCMap;

	/** Parent group */

	void setParent(AtomGroupDescription * const parent);

	const AtomConstraintDescription *
    getAtomConstraintInherits(const ConstraintDescriptionId id) const;

	AtomGroupDescription *					myParent;

	AtomsContainer		              myAtoms;
	GroupsContainer                 myGroups;

	AtomCMap myAtomConstraints;

};

}
}

#endif /* ATOM_GROUP_DESCRIPTION_H */
