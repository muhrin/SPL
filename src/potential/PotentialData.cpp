/*
 * PotentialData.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "potential/PotentialData.h"

// NAMESPACES ////////////////////////////////


namespace sstbx {
namespace potential {

PotentialData::PotentialData():
numParticles(0),
internalEnergy(0.0)
{
}

PotentialData::PotentialData(const sstbx::common::Structure & structure)
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

