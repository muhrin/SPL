/*
 * StructureConstraintDescription.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#include "build_cell/StructureConstraintDescription.h"

namespace sstbx { namespace build_cell {

/**
/* Standard constructor, structure constraints cannot be inherited.
*/
	StructureConstraintDescription::StructureConstraintDescription(const ConstraintDescriptionId type):
AbstractConstraintDescription(type, false)
{

}

}}