/*
 * Minsep.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#include "Minsep.h"

#include "ConstraintDescriptionId.h"

namespace sstbx { namespace build_cell {

Minsep::Minsep(const double minsep, const bool inherits):
myMinsep(minsep),
AtomConstraintDescription(MINSEP, inherits)
{}

double Minsep::getMinsep() const
{
	return myMinsep;
}


}}