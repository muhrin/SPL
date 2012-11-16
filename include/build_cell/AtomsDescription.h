/*
 * AtomsDescription.h
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#ifndef ATOMS_DESCRIPTION_H
#define ATOMS_DESCRIPTION_H

// INCLUDES ///////////////////

#include <string>

#include <boost/optional.hpp>
#include <boost/ptr_container/ptr_map.hpp>

#include "build_cell/IAtomConstrainable.h"
#include "build_cell/AtomGroupDescription.h"
#include "common/AtomSpeciesId.h"

// FORWARD DECLARES ///////////
namespace sstbx {
namespace build_cell {
class AtomConstraintDescription;
class AtomGroupDescription;
}
}

namespace sstbx
{
namespace build_cell
{

class AtomsDescription : public IAtomConstrainable
{
public:

	friend class AtomGroupDescription;

	AtomsDescription();
	AtomsDescription(
    const ::sstbx::common::AtomSpeciesId::Value elementType,
    const size_t elementCount = 1);
	virtual ~AtomsDescription() {}

	const ::sstbx::common::AtomSpeciesId::Value & getSpecies() const;

	void setElementType(const ::sstbx::common::AtomSpeciesId::Value species);

	size_t getCount() const;
	void setCount(const size_t newCount);

  ::boost::optional<double> getRadius() const;
  void setRadius(const double radius);

  const AtomGroupDescription * getParent() const;

	// From IAtomConstrainable /////////////////
	virtual const AtomConstraintDescription *
    getAtomConstraint(const ConstraintDescriptionId id) const;
	virtual void addAtomConstraint(AtomConstraintDescription * const atomConstraint);
	virtual bool removeAtomConstraint(const AtomConstraintDescription * const atomConstraint);
  // End from IAtomConstrainable ////////////

	template <class CType>
	const CType * getAtomConstraint(const ConstraintDescriptionId id) const;

private:

  typedef ::boost::ptr_map<const ConstraintDescriptionId, AtomConstraintDescription> AtomCMap;

	void setParent(const AtomGroupDescription * const parent);

  ::sstbx::common::AtomSpeciesId::Value	mySpecies;
	const AtomGroupDescription *          myParent;
  ::boost::optional<double>             myRadius;

	/**
	/* The number of this type of atom represented.
	*/
	size_t                                myCount;

	/**
	/* The constraints applied to this/these atom(s).
	*/
	AtomCMap                              myAtomConstraints;

};

// IMPLEMENTATION /////////////////////////////////////////

template <class CType>
const CType * AtomsDescription::getAtomConstraint(const ConstraintDescriptionId id) const
{
	//TODO: Check if this could be done by calling getAtomConstraints (non generic)
	const AtomConstraintDescription * constraint = NULL;

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

	return static_cast<const CType *>(constraint);
}

}}

#endif /* ATOMS_DESCRIPTION_H */
