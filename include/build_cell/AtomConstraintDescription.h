/*
 * AtomConstraintDescription.h
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#ifndef ATOM_CONSTRAINT_DESCRIPTION_H
#define ATOM_CONSTRAINT_DESCRIPTION_H

// INCLUDES ///////////////////

#include "build_cell/AbstractConstraintDescription.h"

// FORWARD DECLARES ///////////


namespace sstbx
{
namespace build_cell
{

class AtomConstraintDescription : public AbstractConstraintDescription
{
public:
	/** Standard constructor, by default atom constraints will be inherited */
	AtomConstraintDescription(const ConstraintDescriptionId type, const bool inherit = true);
};

}
}

#endif /* ATOM_CONSTRAINT_DESCRIPTION_H */
