/*
 * IConstraintDescription.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_CONSTRAINT_DESCRIPTION_H
#define I_CONSTRAINT_DESCRIPTION_H

// INCLUDES ////////////
#include "build_cell/ConstraintDescriptionId.h"

// FORWARD DECLARATIONS ///////

namespace sstbx { namespace build_cell {

class IConstraintDescription
{
public:

	virtual ~IConstraintDescription() {}

	virtual ConstraintDescriptionId getType() const = 0;

	virtual bool inherits() const = 0;
};

}}

#endif /* I_CONSTRAINT_DESCRIPTION_H */
