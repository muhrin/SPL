/*
 * PotentialData.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "spl/potential/PotentialData.h"

// NAMESPACES ////////////////////////////////

namespace spl {
namespace potential {

PotentialData::PotentialData()
{
  reset();
}

void
PotentialData::reset()
{
  internalEnergy = 0.0;
  forces.zeros();
  stressMtx.zeros();
}

}
}

