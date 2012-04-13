/*
 * RandomCellGenerator.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef RANDOM_CELL_GENERATOR_H
#define RANDOM_CELL_GENERATOR_H

// INCLUDES //////////////////////////////////////////////
#include "ICellGenerator.h"

// DEFINES ///////////////////////////////////////////////


// FORWARD DECLARES //////////////////////////////////////
namespace sstbx { namespace build_cell {
	class CellDescription;
	template <typename FloatType>
	class RandomCellDescription;
}}

namespace sstbx { namespace build_cell {

template <typename FloatType = double>
class RandomCellGenerator : public ICellGenerator<FloatType>
{
public:

	static const FloatType DEFAULT_VOLUME;
	static const FloatType DEFAULT_VOL_DELTA;
	static const FloatType DEFAULT_MAX_LENGTH_RATIO;
	static const FloatType DEFAULT_MIN_ANGLE;
	static const FloatType DEFAULT_MAX_ANGLE;
	static const FloatType MIN_RAND_LATTICE_LENGTH;

	RandomCellGenerator(const RandomCellDescription<FloatType> & cellDescription);

	virtual ::sstbx::common::AbstractFmidCell<FloatType> * generateCell() const;

private:

	enum LatticeParams {A, B, C, ALPHA, BETA, GAMMA, TOTAL_PARAMS};

	FloatType generateAngle() const;

	FloatType generateLength(const FloatType a, const FloatType b = 0.0) const;

	FloatType generateVolume(const FloatType currentVolume) const;

	// Volume
	FloatType myVolume;
	FloatType myVolDelta;

	// Shape
	FloatType myMaxLengthRatio;
	/** Lattice lengths */
	FloatType myA, myB, myC;
	/** Lattice angles */
	FloatType myAlpha, myBeta, myGamma;

	// Angles
	FloatType myMinAngle;
	FloatType myMaxAngle;
};

}}

// IMPLEMENTATION //////////////////////////////////////

// INCLUDES //////////////////////////////////////////
#include<iostream>

#include "RandomCellDescription.h"
#include "common/AbstractFmidCell.h"
#include "../common/UnitCellFactory.h"
#include "../common/Utils.h"

#define UNSET 0.0

namespace sstbx { namespace build_cell {

// Constants ///////////////////////////////////////////////////////////
template <typename FloatType>
const FloatType RandomCellGenerator<FloatType>::DEFAULT_VOLUME		= 1.0;
template <typename FloatType>
const FloatType RandomCellGenerator<FloatType>::DEFAULT_VOL_DELTA	= 0.2;
template <typename FloatType>
const FloatType RandomCellGenerator<FloatType>::DEFAULT_MAX_LENGTH_RATIO = 2.0;
template <typename FloatType>
const FloatType RandomCellGenerator<FloatType>::DEFAULT_MIN_ANGLE = 45;
template <typename FloatType>
const FloatType RandomCellGenerator<FloatType>::DEFAULT_MAX_ANGLE = 135;
template <typename FloatType>
const FloatType RandomCellGenerator<FloatType>::MIN_RAND_LATTICE_LENGTH = 1e-4;

template <typename FloatType>
RandomCellGenerator<FloatType>::RandomCellGenerator(const RandomCellDescription<FloatType> & cellDescription):
myVolume(UNSET),
myVolDelta(UNSET),
myMaxLengthRatio(UNSET),
myA(UNSET),
myB(UNSET),
myC(UNSET),
myAlpha(UNSET),
myBeta(UNSET),
myGamma(UNSET),
myMinAngle(UNSET),
myMaxAngle(UNSET)
{
	if(cellDescription.myVolume)
	{
		myVolume = *cellDescription.myVolume;
	}
	if(cellDescription.myVolDelta)
	{
		myVolDelta = *cellDescription.myVolDelta;
	}
	if(cellDescription.myMaxLengthRatio)
	{
		myMaxLengthRatio = *cellDescription.myMaxLengthRatio;
	}
	if(cellDescription.myA)
	{
		myA = *cellDescription.myA;
	}
	if(cellDescription.myB)
	{
		myB = *cellDescription.myB;
	}
	if(cellDescription.myC)
	{
		myC = *cellDescription.myC;
	}
	if(cellDescription.myAlpha)
	{
		myAlpha = *cellDescription.myAlpha;
	}
	if(cellDescription.myBeta)
	{
		myBeta = *cellDescription.myBeta;
	}
	if(cellDescription.myGamma)
	{
		myGamma = *cellDescription.myGamma;
	}
	if(cellDescription.myMinAngle)
	{
		myMinAngle = *cellDescription.myMinAngle;
	}
	if(cellDescription.myMaxAngle)
	{
		myMaxAngle = *cellDescription.myMaxAngle;
	}
}


template <typename FloatType>
::sstbx::common::AbstractFmidCell<FloatType> * RandomCellGenerator<FloatType>::generateCell() const
{
	::sstbx::common::AbstractFmidCell<FloatType> * cell = 0;

	// New set of lattice parameters
	double latticeParams[6];

	// Set the lattice lengths
	latticeParams[A] = myA != UNSET ? myA : 0.5;
	latticeParams[B] = myB != UNSET ? myB : generateLength(latticeParams[A]);
	latticeParams[C] = myC != UNSET ? myC : generateLength(latticeParams[A], latticeParams[B]);

	// Set the lattice angles
	latticeParams[ALPHA]	= myAlpha != UNSET ? myAlpha : generateAngle();
	latticeParams[BETA]		= myBeta != UNSET ? myBeta : generateAngle();
	latticeParams[GAMMA]	= myGamma != UNSET ? myGamma : generateAngle();

	// Make sure the length ratio constraint is adhered to
	if(myMaxLengthRatio != UNSET)
	{
		// Find the shortest lattice vector
		LatticeParams min = latticeParams[A] < latticeParams[B] ? A : B;
		min = latticeParams[min] < latticeParams[C] ? min : C;

		for(size_t i = A; i <= C; ++i)
		{
			if(i != min && (latticeParams[i]/latticeParams[min]) > myMaxLengthRatio)
			{
				// Cap the length at the maximum
				latticeParams[i] = latticeParams[min] * myMaxLengthRatio;
			}
		}
	}

	// Make sure the min/max angle constraint is adhered to
	if(myMinAngle != UNSET)
	{
		for(size_t i = ALPHA; i <= GAMMA; ++i)
		{
			latticeParams[i] = latticeParams[i] < myMinAngle ? myMinAngle : latticeParams[i];
		}
	}
	if(myMaxAngle != UNSET)
	{
		for(size_t i = ALPHA; i <= GAMMA; ++i)
		{
			latticeParams[i] = latticeParams[i] > myMaxAngle ? myMaxAngle : latticeParams[i];
		}
	}

	// Create the unit cell
	cell = new sstbx::common::AbstractFmidCell<FloatType>(latticeParams);

	// Finally set the volume
	cell->setVolume(generateVolume(cell->getVolume()));

	return cell;
}


template <typename FloatType>
FloatType RandomCellGenerator<FloatType>::generateAngle() const
{
	return sstbx::common::randDouble(myMinAngle, myMaxAngle);
}

template <typename FloatType>
FloatType RandomCellGenerator<FloatType>::generateLength(const FloatType a, const FloatType b) const
{
	const FloatType shortest = ::std::max(a, b) / myMaxLengthRatio;
	const FloatType longest = b == 0 ? myMaxLengthRatio * a : ::std::min(a, b) * myMaxLengthRatio;

	return sstbx::common::randDouble(shortest, longest);
}

template <typename FloatType>
FloatType RandomCellGenerator<FloatType>::generateVolume(const FloatType currentVolume) const
{
	// Find the target volume, the volume constraint superceeds a volume
	// inferred by the cell definition
	const FloatType targetVol = myVolume != UNSET ? myVolume : currentVolume;
	const FloatType volDelta = myVolDelta != UNSET ? myVolDelta : 0.0;

	return sstbx::common::randDouble(targetVol * (1.0 - volDelta), targetVol * (1.0 + volDelta));
}

}}

#endif /* RANDOM_CELL_GENERATOR_H */
