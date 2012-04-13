/*
 * StructureDescription.h
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_DESCRIPTION_H
#define STRUCTURE_DESCRIPTION_H

// INCLUDES ///////////////////
#include "AtomGroupDescription.h"

// FORWARD DECLARES ///////////
namespace sstbx { namespace build_cell {
	class StructureConstraintDescription;
}}

namespace sstbx { namespace build_cell {

class StructureDescription : public AtomGroupDescription
{
public:
	virtual ~StructureDescription();

	void addStructureConstraint(StructureConstraintDescription * const structureConstraint);	

	std::vector<StructureConstraintDescription *> const & getStructureConstraints();

	//CellDescription & getCellDescription();

protected:

	std::vector<StructureConstraintDescription *> structureConstraints;

	//CellDescription cellDescription;
};

}}

#endif /* STRUCTURE_DESCRIPTION_H */
