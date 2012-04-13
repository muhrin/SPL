/*
 * AtomConstraintDescription.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#include "AtomConstraintDescription.h"

#include "build_cell/ConstraintDescriptionId.h"

namespace sstbx { namespace build_cell {

AtomConstraintDescription::AtomConstraintDescription(const ConstraintDescriptionId type, const bool inherit):
AbstractConstraintDescription(type, inherit)
{
}

}}