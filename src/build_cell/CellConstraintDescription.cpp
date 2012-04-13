/*
 * CellConstraintDescription.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#include "CellConstraintDescription.h"

namespace sstbx { namespace build_cell {

CellConstraintDescription::CellConstraintDescription(const ConstraintDescriptionId type):
AbstractConstraintDescription(type, false)
{
}

}}