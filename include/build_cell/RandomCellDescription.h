/*
 * RandomCellDescription.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef RANDOM_CELL_DESCRIPTION_H
#define RANDOM_CELL_DESCRIPTION_H

// INCLUDES //////////////////////////////////////////////

// DEFINES ///////////////////////////////////////////////

// FORWARD DECLARES //////////////////////////////////////
namespace sstbx { namespace build_cell {
	template <typename FloatType>
	class RandomCellGenerator;
}}

namespace sstbx { namespace build_cell {

template <typename FloatType = double>
class RandomCellDescription
{
public:

	friend class RandomCellGenerator<FloatType>;

	RandomCellDescription();
	~RandomCellDescription();

	void setVolume(const FloatType volume);
	void setVolDelta(const FloatType volDelta);
	void setMaxLengthRatio(const FloatType maxLengthRatio);
	void setMinAngle(const FloatType minAngle);
	void setMaxAngle(const FloatType maxAngle);
	void setLatticeParams(
		const FloatType a, const FloatType b, const FloatType c,
		const FloatType alpha, const FloatType beta, const FloatType gamma);
	void setLatticeParams(const FloatType (&latticeParams)[6]);

private:

	bool allLatticeParamsSet() const;

	FloatType *	myVolume;
	FloatType * myVolDelta;
	FloatType * myMaxLengthRatio;
	FloatType * myMinAngle;
	FloatType * myMaxAngle;
	/** Lattice lengths */
	FloatType * myA;
	FloatType * myB;
	FloatType * myC;
	/** Lattice angles */
	FloatType * myAlpha;
	FloatType * myBeta;
	FloatType * myGamma;
};

}}

// IMPLEMENTATION ///////////////////////////////////////////

namespace sstbx { namespace build_cell {

template <typename FloatType>
RandomCellDescription<FloatType>::RandomCellDescription():
	myVolume(NULL),
	myVolDelta(NULL),
	myMaxLengthRatio(NULL),
	myMinAngle(NULL),
	myMaxAngle(NULL),
	myA(NULL),
	myB(NULL),
	myC(NULL),
	myAlpha(NULL),
	myBeta(NULL),
	myGamma(NULL)
{}

template <typename FloatType>
RandomCellDescription<FloatType>::~RandomCellDescription()
{
	if(myVolume)
		delete myVolume;
	if(myVolDelta)
		delete myVolDelta;
	if(myMaxLengthRatio)
		delete myMaxLengthRatio;
	if(myMinAngle)
		delete myMinAngle;
	if(myMaxAngle)
		delete myMaxAngle;
	if(myA)
		delete myA;
	if(myB)
		delete myB;
	if(myC)
		delete myC;
	if(myAlpha)
		delete myAlpha;
	if(myBeta)
		delete myBeta;
	if(myGamma)
		delete myGamma;
}

template <typename FloatType>
void RandomCellDescription<FloatType>::setVolume(const FloatType volume)
{
	if(!myVolume)
		myVolume = new FloatType;

	*myVolume = volume;
}

template <typename FloatType>
void RandomCellDescription<FloatType>::setVolDelta(const FloatType volDelta)
{
	if(!myVolDelta)
		myVolDelta = new FloatType;

	*myVolDelta = volDelta;
}

template <typename FloatType>
void RandomCellDescription<FloatType>::setMaxLengthRatio(const FloatType maxLengthRatio)
{
	if(!myMaxLengthRatio)
		myMaxLengthRatio = new FloatType;

	*myMaxLengthRatio = maxLengthRatio;
}

template <typename FloatType>
void RandomCellDescription<FloatType>::setMinAngle(const FloatType minAngle)
{
	if(!myMinAngle)
		myMinAngle = new FloatType;

	*myMinAngle = minAngle;
}

template <typename FloatType>
void RandomCellDescription<FloatType>::setMaxAngle(const FloatType maxAngle)
{
	if(!myMaxAngle)
		myMaxAngle = new FloatType;

	*myMaxAngle = maxAngle;
}

template <typename FloatType>
void RandomCellDescription<FloatType>::setLatticeParams(
	const FloatType a, const FloatType b, const FloatType c,
	const FloatType alpha, const FloatType beta, const FloatType gamma)
{
	if(!myA)
		myA = new FloatType;
	if(!myB)
		myB = new FloatType;
	if(!myC)
		myC = new FloatType;
	if(!myAlpha)
		myAlpha = new FloatType;
	if(!myBeta)
		myBeta = new FloatType;
	if(!myGamma)
		myGamma = new FloatType;

	*myA = a;
	*myB = b;
	*myC = c;
	*myAlpha = alpha;
	*myBeta = beta;
	*myGamma = gamma;
}

template <typename FloatType>
void RandomCellDescription<FloatType>::setLatticeParams(const FloatType (&latticeParams)[6])
{
	if(!myA)
		myA = new FloatType;
	if(!myB)
		myB = new FloatType;
	if(!myC)
		myC = new FloatType;
	if(!myAlpha)
		myAlpha = new FloatType;
	if(!myBeta)
		myBeta = new FloatType;
	if(!myGamma)
		myGamma = new FloatType;

	*myA = latticeParams[0];
	*myB = latticeParams[1];
	*myC = latticeParams[2];
	*myAlpha	= latticeParams[3];
	*myBeta		= latticeParams[4];
	*myGamma	= latticeParams[5];
}

template <typename FloatType>
bool RandomCellDescription<FloatType>::allLatticeParamsSet() const
{
	return myA && myB && myC && myAlpha && myBeta && myGamma;
}

}}

#endif /* RANDOM_CELL_DESCRIPTION_H */
