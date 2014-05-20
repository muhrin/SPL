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

PotentialData::PotentialData(const size_t numParticles):
    forces(3, numParticles)
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

