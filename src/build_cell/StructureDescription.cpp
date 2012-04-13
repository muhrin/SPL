/*
 * StructureDescription.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#include "StructureDescription.h"

#include "AtomConstraintDescription.h"
#include "AtomsDescription.h"
#include "StructureConstraintDescription.h"

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