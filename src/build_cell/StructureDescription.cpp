/*
 * StructureDescription.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#include "build_cell/StructureDescription.h"

#include "build_cell/AtomConstraintDescription.h"
#include "build_cell/AtomsDescription.h"
#include "build_cell/StructureConstraintDescription.h"

namespace sstbx { namespace build_cell {

StructureDescription::~StructureDescription()
{
	using std::vector;

	for(vector<StructureConstraintDescription *>::iterator it = structureConstraints.begin(),
		end = structureConstraints.end(); it != end; ++it)
	{
		delete *it;
	}
}

void StructureDescription::addStructureConstraint(StructureConstraintDescription * const structureConstraint)
{
	structureConstraints.push_back(structureConstraint);
}

::std::vector<StructureConstraintDescription *> const & StructureDescription::getStructureConstraints()
{
	return structureConstraints;
}

}}