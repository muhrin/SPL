/*
 * Minsep.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef MINSEP_H
#define MINSEP_H

// INCLUDES /////////////////////////////////////////////////////
#include "AtomConstraintDescription.h"

// FORWARD DECLARES /////////////////////////////////////////////

namespace sstbx { namespace build_cell {

class Minsep : public AtomConstraintDescription
{
public:
	Minsep(const double minsep, const bool inherits = true);
	virtual ~Minsep() {}

	double getMinsep() const;

private:

	const double myMinsep;

};

}}

#endif /* MINSEP_H */
