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

PotentialData::PotentialData():
numParticles(0),
internalEnergy(0.0)
{
}

PotentialData::PotentialData(const spl::common::Structure & structure)
{
	numParticles = structure.getNumAtoms();
	internalEnergy = 0.0;
	structure.getAtomPositions(pos);
	forces.set_size(3, numParticles);
	forces.zeros();
	stressMtx.zeros();
}

}
}

