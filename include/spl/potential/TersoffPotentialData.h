/*
 * TersoffPotentialData.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef TERSOFF_POTENTIAL_DATA_H
#define TERSOFF_POTENTIAL_DATA_H

// INCLUDES /////////////////////////////////////////////
#include "PotentialData.h"

#include "spl/common/Structure.h"

// FORWARD DECLARATIONS ////////////////////////////////////


// DEFINITION //////////////////////////////////////////////

namespace spl { namespace potential {

template <typename FloatType = double>
struct TersoffPotentialData : public PotentialData<FloatType>
{
	TersoffPotentialData(const spl::common::Structure & structure);

};

// IMPLEMENTATION //////////////////////////////////////////
template <typename FloatType>
TersoffPotentialData<FloatType>::TersoffPotentialData(const spl::common::Structure & structure):
PotentialData(structure)
{
}

}}

#endif /* TERSOFF_POTENTIAL_DATA_H */
