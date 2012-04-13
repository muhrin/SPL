/*
 * IAtomConstrainable.h
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_ATOM_CONSTRAINABLE_H
#define I_ATOM_CONSTRAINABLE_H

// INCLUDES ///////////////////
#include "ConstraintDescriptionId.h"

#include <vector>

// FORWARD DECLARES ///////////
namespace sstbx { namespace build_cell {
	enum ConstraintDescriptionId;
	class AtomConstraintDescription;
}}

namespace sstbx { namespace build_cell {

class IAtomConstrainable
{
public:
	virtual void addAtomConstraint(AtomConstraintDescription * const atomConstraint) = 0;

	virtual bool removeAtomConstraint(AtomConstraintDescription * const atomConstraint) = 0;

	virtual AtomConstraintDescription * getAtomConstraint(const ConstraintDescriptionId id) const = 0;

};

}}

#endif /* I_ATOM_CONSTRAINABLE_H */
