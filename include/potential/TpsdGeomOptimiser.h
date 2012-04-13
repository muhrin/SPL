/*
 * TpsdGeomOptimiser.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef TPSD_GEOM_OPTIMISER_H
#define TPSD_GEOM_OPTIMISER_H

// INCLUDES /////////////////////////////////////////////
#include "IGeomOptimiser.h"
#include "IPotential.h"

#include "common/Structure.h"

#include <armadillo>

#include <limits>

// DEFINES //////////////////////////////////////////////
#define TPSD_GEOM_OPTIMISER_TPARAMS	<typename FloatType, class DataType>
#define TPSD_GEOM_OPTIMISER_TTYPE	TpsdGeomOptimiser<FloatType, DataType>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx { namespace potential {

template <typename FloatType = double, class DataType = StandardData<FloatType> >
class TpsdGeomOptimiser : public IGeomOptimiser
{
public:
	TpsdGeomOptimiser(
		const IPotential<DataType> & potential,
		const size_t maxSteps = DEFAULT_MAX_STEPS,
		const FloatType tolerance = DEFAULT_TOLERANCE);

	// IGeomOptimiser interface //////////////////////////////

	virtual const IPotential<DataType> & getPotential() const;

	virtual bool optimise(::sstbx::common::Structure & structure) const;

	virtual bool optimise(
		::sstbx::common::Structure & structure,
		StandardData<FloatType> * & data) const;

	// End IGeomOptimiser interface

	bool optimise(
		::sstbx::common::Structure & structure,
		DataType & data,
		const size_t maxSteps,
		const FloatType eTol) const;

	static const size_t		DEFAULT_MAX_STEPS;
	static const FloatType	DEFAULT_TOLERANCE;
	static const FloatType	DEFAULT_CELL_ANGLE_THRESHOLD;
	static const FloatType	MAX_DH_TO_H_RATIO;

private:

	const IPotential<DataType> & myPotential;

	const size_t		myMaxSteps;
	const FloatType		myTolerance;
};

// CONSTANTS ////////////////////////////////////////////////

template TPSD_GEOM_OPTIMISER_TPARAMS
const size_t TPSD_GEOM_OPTIMISER_TTYPE::DEFAULT_MAX_STEPS = 1000;

template TPSD_GEOM_OPTIMISER_TPARAMS
const FloatType TPSD_GEOM_OPTIMISER_TTYPE::DEFAULT_TOLERANCE = 1e-13;

template TPSD_GEOM_OPTIMISER_TPARAMS
const FloatType TPSD_GEOM_OPTIMISER_TTYPE::MAX_DH_TO_H_RATIO = 10000;

template TPSD_GEOM_OPTIMISER_TPARAMS
const FloatType TPSD_GEOM_OPTIMISER_TTYPE::DEFAULT_CELL_ANGLE_THRESHOLD = 5;

// IMPLEMENTATION //////////////////////////////////////////////////////////

template TPSD_GEOM_OPTIMISER_TPARAMS
TPSD_GEOM_OPTIMISER_TTYPE::TpsdGeomOptimiser(
	const IPotential<DataType> & potential,
	const size_t maxSteps,
	const FloatType tolerance):
myPotential(potential),
myMaxSteps(maxSteps),
myTolerance(tolerance)
{}

template TPSD_GEOM_OPTIMISER_TPARAMS
const IPotential<DataType> & TPSD_GEOM_OPTIMISER_TTYPE::getPotential() const
{
	return myPotential;
}

template TPSD_GEOM_OPTIMISER_TPARAMS
bool TPSD_GEOM_OPTIMISER_TTYPE::optimise(
	::sstbx::common::Structure & structure) const
{
	DataType * data = myPotential.generatePotentialData(structure);
	const bool outcome = optimise(structure, *data, DEFAULT_MAX_STEPS, DEFAULT_TOLERANCE);

	// Copy the new positions back to the structure
	structure.setAtomPositionsDescendent(data->pos);

	// Clean up
	delete data;
	data = NULL;

	return outcome;
}

template TPSD_GEOM_OPTIMISER_TPARAMS
bool TPSD_GEOM_OPTIMISER_TTYPE::optimise(
	::sstbx::common::Structure & structure,
	StandardData<FloatType> * & data) const
{
	DataType * const geomData = myPotential.generatePotentialData(structure);
	const bool outcome = optimise(structure, *geomData, DEFAULT_MAX_STEPS, DEFAULT_TOLERANCE);

	// Copy the new positions back to the structure
	structure.setAtomPositionsDescendent(geomData->pos);

	// Passing data out to caller - ownership is transferred and they
	// should clean up!
	data = geomData;
	return outcome;
}

template TPSD_GEOM_OPTIMISER_TPARAMS
bool TPSD_GEOM_OPTIMISER_TTYPE::optimise(
	::sstbx::common::Structure & structure,
	DataType & data,
	const size_t maxSteps,
	const FloatType eTol) const
{
	using namespace arma;

	typedef typename arma::Col<FloatType>::template fixed<3>	Vec3;
	typedef typename arma::Mat<FloatType>						Mat3;
	typedef typename arma::Mat<FloatType>::template fixed<3, 3>	Mat33;

	FloatType h, h0, dH;

	// Stress matrices
	Mat33	s, s0, deltaS, deltaLatticeCar;
	// Position matrices, current are in data.myPos
	Mat3 deltaPos(3, data.numParticles);
	// Forces, current are in data.myForces
	Mat3	f0(3, data.numParticles), deltaF(3, data.numParticles);

	sstbx::common::AbstractFmidCell<FloatType> & uc = data.unitCell;

	Mat33 latticeCar;
	FloatType gamma, volume, volumeSq, gammaNumIonsOVolume;
	FloatType xg, gg;
	Vec3 f;

	data.forces.fill(1.0);
	deltaPos.fill(0.0);
	deltaLatticeCar.fill(0.0);
	latticeCar			= uc.getOrthoMtx();


	// Initialisation of variables
	dH	= std::numeric_limits<FloatType>::max();
	h	= 1.0;
	s.fill(1.0);

	const size_t numIonsSq	= data.numParticles * data.numParticles;

	bool converged = false;
	// Set the initial step size so get mooving
	FloatType step = 0.001;
	for(size_t i = 0; !converged && i < maxSteps; ++i)
	{
		h0 = h;
		f0 = data.forces;
		s0 = s;

		volume		= uc.getVolume();
		volumeSq	= volume * volume;

		// Evaluate the potential
		myPotential.evalPotential(structure, data);

		// Now balance forces
		// (do sum of values for each component and divide by number of particles)
		f = sum(data.forces, 1) / data.numParticles;

		data.forces.row(0) += f(0);
		data.forces.row(1) += f(1);
		data.forces.row(2) += f(2);

		// TODO: Check this gamma = 0 as it seems a little odd
		gamma = 0.0;
		data.totalEnthalpy += gamma * numIonsSq / volume;

		gammaNumIonsOVolume = gamma * numIonsSq / volumeSq;
		data.stressMtx.diag() += gammaNumIonsOVolume;

		s = data.stressMtx * latticeCar;

		// TODO: ADD BACK IN EXTERNAL PRESSURE!
		h = data.totalEnthalpy /*+ trace(data.myExternalPressure) / 3.0 * volume*/;

		deltaF	= data.forces - f0;
		// The accu function will do the sum of all elements
		// and the % operator does the Shure product i.e.
		// element wise multiplication of two matrices
		xg		= accu(deltaPos % deltaF);
		gg		= accu(deltaF % deltaF);

		deltaS	= s - s0;
		xg		+= accu(deltaLatticeCar % deltaS);
		gg		+= accu(deltaS % deltaS);


		if(fabs(xg) > 0.0)
			step = fabs(xg / gg);

		// Move the particles on by a step, saving the old positions
		deltaPos		= step * data.forces;
		data.pos		+= deltaPos;

		// Fractionalise coordinates
		const Mat33 & fracMtx = uc.getFracMtx();
		data.pos		= fracMtx * data.pos;
		data.pos		-= arma::floor(data.pos);

		// Move on cell vectors to relax strain
		// TODO: ADD BACK EXTERNAL PRESSURE
		deltaLatticeCar = step * (s /*- data.myExternalPressure * latticeCar*/);
		latticeCar		+= deltaLatticeCar;
		uc.setOrthoMtx(latticeCar);

		// Finally re-orthogonalise the ion positions
		data.pos		= latticeCar * data.pos;

		dH = h - h0;

		converged = fabs(dH) < eTol;


		// TODO: Temporary escape condition to catch pathological cases!
		if(dH / h > MAX_DH_TO_H_RATIO)
			break;

		const double (&latticeParams)[6] = data.unitCell.getLatticeParams();
		if(latticeParams[3] < DEFAULT_CELL_ANGLE_THRESHOLD || latticeParams[3] > (180 - DEFAULT_CELL_ANGLE_THRESHOLD))
			break;
		if(latticeParams[4] < DEFAULT_CELL_ANGLE_THRESHOLD || latticeParams[4] > (180 - DEFAULT_CELL_ANGLE_THRESHOLD))
			break;
		if(latticeParams[5] < DEFAULT_CELL_ANGLE_THRESHOLD || latticeParams[5] > (180 - DEFAULT_CELL_ANGLE_THRESHOLD))
			break;
	}

	// Wrap the particle positions so they stay in the central unit cell
	data.unitCell.wrapVecsInplace(data.pos);

	return converged;

}

}}

#endif /* TPSD_GEOM_OPTIMISER_H */
