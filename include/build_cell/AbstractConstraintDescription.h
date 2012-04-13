/*
 * AbstractConstraintDescription.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef ABSTRACT_CONSTRAINT_DESCRIPTION_H
#define ABSTRACT_CONSTRAINT_DESCRIPTION_H

// INCLUDES ////////////
#include "build_cell/IConstraintDescription.h"
#include "build_cell/ConstraintDescriptionId.h"

// FORWARD DECLARATIONS ///////


// DEFINITION ///////////////////////

namespace sstbx { namespace build_cell {

class AbstractConstraintDescription : public IConstraintDescription
{
public:

	AbstractConstraintDescription(const ConstraintDescriptionId name, const bool inherits);

	/** Is this constraint inherited by elements further down the structure
	/*  description tree.
	*/
	virtual bool inherits() const;

	virtual ConstraintDescriptionId getType() const;

private:

	/** Should this constraint be inherited down the structure description
	/*  tree.	*/
	const bool myInherits;

	const ConstraintDescriptionId myType;
};

}}


#endif /* ABSTRACT_CONSTRAINT_DESCRIPTION_H */
