/*
 * StandardData.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef STANDARD_DATA_H
#define STANDARD_DATA_H

// INCLUDES /////////////////////////////////////////////
#include "common/Structure.h"

#include <armadillo>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx { namespace potential {

template <typename FloatType = double>
struct StandardData
{
	StandardData(sstbx::common::Structure & structure);

	sstbx::common::AbstractFmidCell<FloatType> & unitCell;
	std::size_t							numParticles;
	FloatType							totalEnthalpy;
	typename arma::Mat<FloatType>		pos;
	typename arma::Mat<FloatType>		forces;
	typename arma::Mat<FloatType>::template fixed<3, 3>	stressMtx;
};

// IMPLEMENTATION //////////////////////////////////////////

template <typename FloatType>
StandardData<FloatType>::StandardData(sstbx::common::Structure & structure):
unitCell(*structure.getUnitCell())
{
	numParticles = structure.getNumAtomsDescendent();
	totalEnthalpy = 0.0;
	structure.getAtomPositionsDescendent(pos);
	forces.set_size(3, numParticles);
	forces.fill(0.0);
	stressMtx.fill(0.0);
}

}}

#endif /* STANDARD_DATA_H */