/*
 * CellConstraintDescription.h
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#ifndef CELL_CONSTRAINT_DESCRIPTION_H
#define CELL_CONSTRAINT_DESCRIPTION_H

// INCLUDES ///////////////////

#include "AbstractConstraintDescription.h"

// FORWARD DECLARES ///////////

namespace sstbx { namespace build_cell {


class CellConstraintDescription : public AbstractConstraintDescription
{
public:
	/** Standard constructor, by default atom constraints will be inherited */
	CellConstraintDescription(const ConstraintDescriptionId type);
};

}}

#endif /* CELL_CONSTRAINT_DESCRIPTION_H */
