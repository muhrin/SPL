/*
 * StructureConstraintDescription.h
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_CONSTRAINT_DESCRIPTION_H
#define STRUCTURE_CONSTRAINT_DESCRIPTION_H

// INCLUDES ///////////////////
#include "build_cell/AbstractConstraintDescription.h"

// FORWARD DECLARES ///////////

namespace sstbx { namespace build_cell {

class StructureConstraintDescription : AbstractConstraintDescription
{
public:
	StructureConstraintDescription(const ConstraintDescriptionId type);

};

}}

#endif /* STRUCTURE_CONSTRAINT_DESCRIPTION_H */
