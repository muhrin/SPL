/*
 * AbstractFmidCell.h
 *
 * Abstract Fast Minimum Image Cell
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef ABSTRACT_FMID_CELL_H
#define ABSTRACT_FMID_CELL_H

// INCLUDES /////////////////////////////////////////////
#include "SSLib.h"

#include <vector>
#include <math.h>

#include <armadillo>

#include <cctbx/uctbx.h>
#include <cctbx/uctbx/fast_minimum_reduction.h>

#include "common/Constants.h"

// DEFINES ////////////////////////////////////////////////
#define ABSTRACT_FMID_CELL AbstractFmidCell<FloatType>
#define ABSTRACT_FMID_CELL_TYPE typename AbstractFmidCell<FloatType>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx { namespace common {

template<typename FloatType = double>
class AbstractFmidCell
{
public:

	// TYPEDEFS ////////////////////////
	// Need to use the typename keyword to make it clear that these are types
	typedef typename arma::Mat<FloatType>				Mat;
	typedef typename arma::Mat<FloatType>::template fixed<3, 3>	Mat33;
	typedef typename arma::Col<FloatType>::template fixed<3>		Vec3;

	// Copy constructor
	AbstractFmidCell(const AbstractFmidCell<FloatType> & toCopy);

	AbstractFmidCell(
		const FloatType a, const FloatType b, const FloatType c,
		const FloatType alpha, const FloatType beta, const FloatType gamma);

	AbstractFmidCell(const FloatType (&latticeParams)[6]);

	cctbx::uctbx::unit_cell * asCctbxUnitCell() const;

	/**
	/* Get the minimum distance between two cartesian points respecting the boundary
	/* conditions
	/**/
	virtual FloatType getDistanceMinimumImg(
		const typename AbstractFmidCell<FloatType>::Vec3 & a,
		const typename AbstractFmidCell<FloatType>::Vec3 & b) const;

	virtual FloatType getDistanceSqMinimumImg(
		const typename AbstractFmidCell<FloatType>::Vec3 & a,
		const typename AbstractFmidCell<FloatType>::Vec3 & b) const;

	const FloatType (&getLatticeParams() const)[6];

	virtual typename AbstractFmidCell<FloatType>::Vec3 getVecMinimumImg(
		const typename AbstractFmidCell<FloatType>::Vec3 & a,
		const typename AbstractFmidCell<FloatType>::Vec3 & b) const;

	virtual void getAllVectorsWithinCutoff(
		const typename AbstractFmidCell<FloatType>::Vec3 & cart1,
		const typename AbstractFmidCell<FloatType>::Vec3 & cart2,
		const FloatType cutoff,
		std::vector<typename AbstractFmidCell<FloatType>::Vec3> & outVectors) const;

	virtual void getAllDistancesWithinCutoff(
		const typename AbstractFmidCell<FloatType>::Vec3 & cart1,
		const typename AbstractFmidCell<FloatType>::Vec3 & cart2,
		const FloatType cutoff,
		std::vector<FloatType> & outVectors,
		const size_t maxCellMultiples = 0) const;

	FloatType getNumPlaneRepetitionsToBoundSphere(
		const typename AbstractFmidCell<FloatType>::Vec3 & boundDir,
		const typename AbstractFmidCell<FloatType>::Vec3 & planeVecA,
		const typename AbstractFmidCell<FloatType>::Vec3 & planeVecB,
		const FloatType radius) const;

	typename AbstractFmidCell<FloatType>::Vec3 wrapVec(const typename AbstractFmidCell<FloatType>::Vec3 & toWrap) const;
	typename AbstractFmidCell<FloatType>::Vec3 & wrapVecInplace(typename AbstractFmidCell<FloatType>::Vec3 & toWrap) const;

	typename AbstractFmidCell<FloatType>::Mat & wrapVecsInplace(typename AbstractFmidCell<FloatType>::Mat & carts) const;
	typename AbstractFmidCell<FloatType>::Mat & wrapVecsInplaceFrac(typename AbstractFmidCell<FloatType>::Mat & fracs) const;

	typename AbstractFmidCell<FloatType>::Vec3 wrapVecFrac(const typename AbstractFmidCell<FloatType>::Vec3 & toWrap) const;
	typename AbstractFmidCell<FloatType>::Vec3 & wrapVecFracInplace(typename AbstractFmidCell<FloatType>::Vec3 & toWrap) const;

	virtual Vec3 randomPoint() const;

	/////////////////////////////////////////////////////////
	// VECTOR TRANSFORMATION METHODS ////////////////////////

	virtual Vec3 fractionalise(const typename AbstractFmidCell<FloatType>::Vec3 & cartesian) const;

	virtual Vec3 & fractionaliseInplace(typename AbstractFmidCell<FloatType>::Vec3 & cart) const;

	/**
	/* Fractionalise multiple cartesian vectors.  This method takes an argument that
	/* must be a 3 x n matrix.
	/**/
	virtual Mat & fractionaliseInplace(typename AbstractFmidCell<FloatType>::Mat & carts) const;

	virtual Vec3 orthogonalise(const typename AbstractFmidCell<FloatType>::Vec3 & frac) const;

	virtual Vec3 & orthogonaliseInplace(typename AbstractFmidCell<FloatType>::Vec3 & frac) const;

	virtual Mat & orthogonaliseInplace(typename AbstractFmidCell<FloatType>::Mat & fracs) const;

	// END ORTHOGONALISATION METHODS //////////////////////////////

	virtual const Mat33 & getOrthoMtx() const;

	virtual void setOrthoMtx(const typename AbstractFmidCell<FloatType>::Mat33 & orthoMtx);
	const Mat33 getFracMtx() const;
	/**
	/* To do fast minimum image calculations a change of basis may have been performed,
	/* in which case this call will return the change of basis transformation matrix.
	/* */
	virtual const Mat33 & getTransformationMtx() const;

	// TODO: RENAME THESE!
	FloatType getLongestVector() const;
	FloatType getLongestVectorSq() const;

	virtual FloatType getVolume() const;

	void setVolume(const FloatType volume);

	typename AbstractFmidCell<FloatType>::Mat33 compactNiggli();

protected:

	/** Initialise the unit cell from lattice parameters */
	void init(
		const FloatType a, const FloatType b, const FloatType c,
		const FloatType alpha, const FloatType beta, const FloatType gamma);
	/** Initialise the unit cell from an orthogonalisation matrix */
	void init(const Mat33 & orthoMtx);
	void initOrthoAndFracMatrices();
	void initLatticeParams();
	void initRest();

	/** The unit cell matrix where columns represent basis vectors */
	typename AbstractFmidCell<FloatType>::Mat33 myOrthoMtx;

	/** The inverse of the orthogonalisation matrix */
	typename AbstractFmidCell<FloatType>::Mat33 myFracMtx;

	typename AbstractFmidCell<FloatType>::Mat33 myChangeOfBasisMtx;

	FloatType	myLatticeParams[6];

	FloatType	myVolume;
};

// IMPLEMENTATION /////////////////////////////////////////////

template <typename FloatType>
AbstractFmidCell<FloatType>::AbstractFmidCell(const AbstractFmidCell<FloatType> & toCopy)
{
	init(toCopy.getOrthoMtx());

	// Identity change of basis matrix
	myChangeOfBasisMtx.eye();
}


template <typename FloatType>
AbstractFmidCell<FloatType>::AbstractFmidCell(
	const FloatType a, const FloatType b, const FloatType c,
	const FloatType alpha, const FloatType beta, const FloatType gamma)
{
	init(a, b, c, alpha, beta, gamma);

	// Identity change of basis matrix
	myChangeOfBasisMtx.eye();
}

template <typename FloatType>
AbstractFmidCell<FloatType>::AbstractFmidCell(const FloatType (&latticeParams)[6])
{
	// TODO: Tidy this up a bit
	init(latticeParams[0], latticeParams[1], latticeParams[2],
		latticeParams[3], latticeParams[4], latticeParams[5]);

	// Identity change of basis matrix
	myChangeOfBasisMtx.eye();
}

template <typename FloatType>
cctbx::uctbx::unit_cell * AbstractFmidCell<FloatType>::asCctbxUnitCell() const
{
	scitbx::af::double6 params;
	memcpy(&params.elems, &myLatticeParams, 6 * sizeof(FloatType));
	return new cctbx::uctbx::unit_cell(params);
}

template <typename FloatType>
FloatType AbstractFmidCell<FloatType>::getDistanceMinimumImg(
	const ABSTRACT_FMID_CELL_TYPE::Vec3 & a, const ABSTRACT_FMID_CELL_TYPE::Vec3 & b) const
{
	return sqrt(getDistanceSqMinimumImg(a, b));
}

template <typename FloatType>
FloatType AbstractFmidCell<FloatType>::getDistanceSqMinimumImg(
	const typename AbstractFmidCell<FloatType>::Vec3 & a,
	const typename AbstractFmidCell<FloatType>::Vec3 & b) const
{
	using namespace arma;
	typename AbstractFmidCell<FloatType>::Vec3 dr = getVecMinimumImg(a, b);
	return dot(dr, dr);
}

template <typename FloatType>
const FloatType (&AbstractFmidCell<FloatType>::getLatticeParams() const)[6]
{
	return myLatticeParams;
}

template <typename FloatType>
typename AbstractFmidCell<FloatType>::Vec3 AbstractFmidCell<FloatType>::getVecMinimumImg(
	const typename AbstractFmidCell<FloatType>::Vec3 & a,
	const typename AbstractFmidCell<FloatType>::Vec3 & b) const
{
	using namespace arma;

	// Make sure cart1 and 2 are in the unit cell at the origin
	const typename AbstractFmidCell<FloatType>::Vec3 		aUnit	= wrapVec(a);
	const typename AbstractFmidCell<FloatType>::Vec3 		bUnit	= wrapVec(b);
	const typename AbstractFmidCell<FloatType>::Vec3		dR		= bUnit - aUnit;
	FloatType		minModDRSq= dot(dR, dR);
	const FloatType minModDR= sqrt(minModDRSq);

	// Get the lattice vectors
	const typename AbstractFmidCell<FloatType>::Vec3 A = myOrthoMtx.col(0);
	const typename AbstractFmidCell<FloatType>::Vec3 B = myOrthoMtx.col(1);
	const typename AbstractFmidCell<FloatType>::Vec3 C = myOrthoMtx.col(2);

	// Maximum multiple of cell vectors we need to go to
	int maxA = (int)ceil(getNumPlaneRepetitionsToBoundSphere(A, B, C, minModDR));
	int maxB = (int)ceil(getNumPlaneRepetitionsToBoundSphere(B, A, C, minModDR));
	int maxC = (int)ceil(getNumPlaneRepetitionsToBoundSphere(C, A, B, minModDR));

	typename AbstractFmidCell<FloatType>::Vec3 dFrac, dRImg, minDR = dR;
	FloatType modDRSq;
	for(int a = -maxA; a <= maxA; ++a)
	{
		for(int b = -maxB; b <= maxB; ++b)
		{
			for(int c = -maxC; c <= maxC; ++c)
			{
				// TODO: Replace this with matrix multiplication
				dFrac[0] = a;
				dFrac[1] = b;
				dFrac[2] = c;

				dRImg = orthogonalise(dFrac) + dR;
				
				modDRSq = dot(dRImg, dRImg);
				if(modDRSq < minModDRSq)
				{
					minModDRSq	= modDRSq;
					minDR		= dRImg;
				}
			}
		}
	}

	return minDR;
}

template <typename FloatType>
void AbstractFmidCell<FloatType>::getAllVectorsWithinCutoff(
	const typename AbstractFmidCell<FloatType>::Vec3 & cart1,
	const typename AbstractFmidCell<FloatType>::Vec3 & cart2,
	const FloatType cutoff,
	std::vector<typename AbstractFmidCell<FloatType>::Vec3> & outVectors) const
{
	using namespace arma;

	// Make sure cart1 and 2 are in the unit cell at the origin
	const typename AbstractFmidCell<FloatType>::Vec3 cart1Unit = wrapVec(cart1);
	const typename AbstractFmidCell<FloatType>::Vec3 cart2Unit = wrapVec(cart2);

	// Get the lattice vectors
	const typename AbstractFmidCell<FloatType>::Vec3 A = myOrthoMtx.col(0);
	const typename AbstractFmidCell<FloatType>::Vec3 B = myOrthoMtx.col(1);
	const typename AbstractFmidCell<FloatType>::Vec3 C = myOrthoMtx.col(2);

	// Maximum multiple of cell vectors we need to go to
	int maxA = (int)ceil(getNumPlaneRepetitionsToBoundSphere(A, B, C, cutoff));
	int maxB = (int)ceil(getNumPlaneRepetitionsToBoundSphere(B, A, C, cutoff));
	int maxC = (int)ceil(getNumPlaneRepetitionsToBoundSphere(C, A, B, cutoff));

	const typename AbstractFmidCell<FloatType>::Vec3		dR			= cart2Unit - cart1Unit;
	const FloatType	cutoffSq	= cutoff * cutoff;

	typename AbstractFmidCell<FloatType>::Vec3 dFrac, dRImg;
	for(int a = -maxA; a <= maxA; ++a)
	{
		for(int b = -maxB; b <= maxB; ++b)
		{
			for(int c = -maxC; c <= maxC; ++c)
			{
				// TODO: Replace this with matrix multiplication
				dFrac[0] = a;
				dFrac[1] = b;
				dFrac[2] = c;

				dRImg = orthogonalise(dFrac) + dR;

				
				if(dot(dRImg, dRImg) < cutoffSq)
				{
					outVectors.push_back(dRImg);
				}
			}
		}
	}
}

/*
template <typename FloatType>
FloatType AbstractFmidCell<FloatType>::getNumPlaneRepetitionsToBoundSphere(
	const typename arma::Row<FloatType>::fixed<3> & boundDir,
	const typename arma::Row<FloatType>::fixed<3> & planeVecA,
	const typename arma::Row<FloatType>::fixed<3> & planeVecB,
	const FloatType radius) const
{
	// The vector normal to the plane
	const typename AbstractFmidCell<FloatType>::Vec3 normal = arma::cross(planeVecA, planeVecB);

	return radius / getVolume() * std::sqrt(arma::dot(normal, normal));
}
*/

template <typename FloatType>
FloatType AbstractFmidCell<FloatType>::getNumPlaneRepetitionsToBoundSphere(
	const typename AbstractFmidCell<FloatType>::Vec3 & boundDir,
	const typename AbstractFmidCell<FloatType>::Vec3 & planeVecA,
	const typename AbstractFmidCell<FloatType>::Vec3 & planeVecB,
	const FloatType radius) const
{
	// The vector normal to the plane
	const typename AbstractFmidCell<FloatType>::Vec3 normal = arma::cross(planeVecA, planeVecB);

	return radius / getVolume() * std::sqrt(arma::dot(normal, normal));
}

template <typename FloatType>
typename AbstractFmidCell<FloatType>::Vec3 AbstractFmidCell<FloatType>::randomPoint() const
{
	// Generate a random point in fractional coords and then
	// orthogonalise it
	typename AbstractFmidCell<FloatType>::Vec3 x;
	x.randu();
	return orthogonaliseInplace(x);
}

template <typename FloatType>
typename AbstractFmidCell<FloatType>::Vec3 AbstractFmidCell<FloatType>::fractionalise(const typename AbstractFmidCell<FloatType>::Vec3 & cartesian) const
{
	return myFracMtx * cartesian;
}

template <typename FloatType>
typename AbstractFmidCell<FloatType>::Vec3 AbstractFmidCell<FloatType>::orthogonalise(const typename AbstractFmidCell<FloatType>::Vec3 & frac) const
{
	return myOrthoMtx * frac;
}

template <typename FloatType>
const typename AbstractFmidCell<FloatType>::Mat33 & AbstractFmidCell<FloatType>::getOrthoMtx() const
{
	return myOrthoMtx;
}

template <typename FloatType>
void AbstractFmidCell<FloatType>::setOrthoMtx(const typename AbstractFmidCell<FloatType>::Mat33 & orthoMtx)
{
	myOrthoMtx = orthoMtx;
	init(orthoMtx);
}

template <typename FloatType>
const typename AbstractFmidCell<FloatType>::Mat33 AbstractFmidCell<FloatType>::getFracMtx() const
{
	return myFracMtx;
}

template <typename FloatType>
const typename AbstractFmidCell<FloatType>::Mat33 & AbstractFmidCell<FloatType>::getTransformationMtx() const
{
	return myChangeOfBasisMtx;
}

template <typename FloatType>
typename AbstractFmidCell<FloatType>::Vec3 AbstractFmidCell<FloatType>::wrapVec(const typename AbstractFmidCell<FloatType>::Vec3 & toWrap) const
{
	using namespace arma;

	// Fractionalise it
	typename AbstractFmidCell<FloatType>::Vec3 frac = fractionalise(toWrap);

	// Wrap it
	wrapVecFracInplace(frac);

	// Orthogonalise it back
	return orthogonaliseInplace(frac);
}

template <typename FloatType>
typename AbstractFmidCell<FloatType>::Vec3 & AbstractFmidCell<FloatType>::wrapVecInplace(typename AbstractFmidCell<FloatType>::Vec3 & toWrap) const
{
	// Fractionalise it
	toWrap = fractionaliseInplace(toWrap);

	// Wrap it
	wrapVecFracInplace(toWrap);

	// Orthogonalise it back
	toWrap = orthogonaliseInplace(toWrap);

	return toWrap;
}

template <typename FloatType>
typename AbstractFmidCell<FloatType>::Mat & AbstractFmidCell<FloatType>::wrapVecsInplace(typename AbstractFmidCell::Mat & toWrap) const
{
	// Make sure we have three rows, otherwise this doesn't make sense
	SSE_ASSERT(toWrap.n_rows == 3);

	for(size_t i = 0; i < toWrap.n_cols; ++i)
	{
		typename AbstractFmidCell<FloatType>::Vec3 vec = toWrap.unsafe_col(i);
		wrapVecInplace(vec);
	}

	return toWrap;
}

template <typename FloatType>
typename AbstractFmidCell<FloatType>::Mat & AbstractFmidCell<FloatType>::wrapVecsInplaceFrac(typename AbstractFmidCell::Mat & fracs) const
{
	// Make sure we have three rows, otherwise this doesn't make sense
	SSE_ASSERT(fracs.n_rows == 3);

	for(size_t i = 0; i < fracs.n_cols; ++i)
	{
		fracs.at(0, i) -= floor(fracs.at(0, i));
		fracs.at(1, i) -= floor(fracs.at(1, i));
		fracs.at(2, i) -= floor(fracs.at(2, i));
	}

	return fracs;
}

template <typename FloatType>
typename AbstractFmidCell<FloatType>::Vec3 AbstractFmidCell<FloatType>::wrapVecFrac(const typename AbstractFmidCell<FloatType>::Vec3 & toWrap) const
{
	// Make a copy
	typename AbstractFmidCell<FloatType>::Vec3 wrapped = toWrap;
	// wrap it
	wrapVecFracInplace(wrapped);
	// give it back
	return wrapped;
}

template <typename FloatType>
typename AbstractFmidCell<FloatType>::Vec3 & AbstractFmidCell<FloatType>::wrapVecFracInplace(typename AbstractFmidCell<FloatType>::Vec3 & toWrap) const
{
	toWrap[0] -= floor(toWrap[0]);
	toWrap[1] -= floor(toWrap[1]);
	toWrap[2] -= floor(toWrap[2]);

	return toWrap;
}

template <typename FloatType>
typename AbstractFmidCell<FloatType>::Vec3 & AbstractFmidCell<FloatType>::fractionaliseInplace(typename AbstractFmidCell<FloatType>::Vec3 & cart) const
{
	// TODO: Look up Armadillo docs to see if there is a *= for post multiplication
	cart = myFracMtx * cart;
	return cart;
}

template <typename FloatType>
typename AbstractFmidCell<FloatType>::Mat & AbstractFmidCell<FloatType>::fractionaliseInplace(Mat & carts) const
{
	SSE_ASSERT(carts.n_rows == 3);

	// TODO: Look up Armadillo docs to see if there is a *= for post multiplication
	carts = myFracMtx * carts;
	return carts;
}


template <typename FloatType>
void AbstractFmidCell<FloatType>::getAllDistancesWithinCutoff(
	const typename AbstractFmidCell<FloatType>::Vec3 & cart1,
	const typename AbstractFmidCell<FloatType>::Vec3 & cart2,
	const FloatType cutoff,
	std::vector<FloatType> & outDistances,
	const size_t maxCellMultiples) const
{
	using namespace arma;
	using std::min;
	using std::max;

	// Make sure cart1 and 2 are in the unit cell at the origin
	//const ABSTRACT_FMID_CELL_TYPE::Vec3 cart1Unit = wrapVec(cart1);
	//const ABSTRACT_FMID_CELL_TYPE::Vec3 cart2Unit = wrapVec(cart2);
	const ABSTRACT_FMID_CELL_TYPE::Vec3	dR	= wrapVec(cart2) - wrapVec(cart1);

	// Get the lattice vectors
	const ABSTRACT_FMID_CELL_TYPE::Vec3 A = myOrthoMtx.col(0);
	const ABSTRACT_FMID_CELL_TYPE::Vec3 B = myOrthoMtx.col(1);
	const ABSTRACT_FMID_CELL_TYPE::Vec3 C = myOrthoMtx.col(2);

	// Maximum multiple of cell vectors we need to go to
	int maxA = (int)ceil(getNumPlaneRepetitionsToBoundSphere(A, B, C, cutoff));
	int maxB = (int)ceil(getNumPlaneRepetitionsToBoundSphere(B, A, C, cutoff));
	int maxC = (int)ceil(getNumPlaneRepetitionsToBoundSphere(C, A, B, cutoff));
	if(maxCellMultiples > 0)
	{
		maxA = min(maxA, (int)maxCellMultiples);
		maxB = min(maxB, (int)maxCellMultiples);
		maxC = min(maxC, (int)maxCellMultiples);
	}

	const FloatType	cutoffSq	= cutoff * cutoff;

	FloatType modDRSq;
	ABSTRACT_FMID_CELL_TYPE::Vec3 dFrac, dRImg;
	for(int a = -maxA; a <= maxA; ++a)
	{
		for(int b = -maxB; b <= maxB; ++b)
		{
			for(int c = -maxC; c <= maxC; ++c)
			{
				// TODO: Replace this with matrix multiplication
				dFrac[0] = a;
				dFrac[1] = b;
				dFrac[2] = c;

				dRImg = orthogonalise(dFrac) + dR;
				modDRSq = dot(dRImg, dRImg);
				if(modDRSq < cutoffSq)
				{
					outDistances.push_back(sqrt(modDRSq));
				}
			}
		}
	}
}

template <typename FloatType>
typename AbstractFmidCell<FloatType>::Vec3 & AbstractFmidCell<FloatType>::orthogonaliseInplace(typename AbstractFmidCell<FloatType>::Vec3 & frac) const
{
	frac = myOrthoMtx * frac;
	return frac;
}

template <typename FloatType>
typename AbstractFmidCell<FloatType>::Mat & AbstractFmidCell<FloatType>::orthogonaliseInplace(Mat & fracs) const
{
	SSE_ASSERT(fracs.n_rows == 3);

	fracs = myOrthoMtx * fracs;
	return fracs;
}

template <typename FloatType>
FloatType AbstractFmidCell<FloatType>::getVolume() const
{
	return myVolume;
}

template <typename FloatType>
void AbstractFmidCell<FloatType>::setVolume(const FloatType volume)
{
	const FloatType scale = pow(volume / getVolume(), 1.0 / 3.0);

	init(scale * myOrthoMtx);
}

template <typename FloatType>
void AbstractFmidCell<FloatType>::init(
	const FloatType a, const FloatType b, const FloatType c,
	const FloatType alpha, const FloatType beta, const FloatType gamma)
{

	myLatticeParams[0] = a;
	myLatticeParams[1] = b;
	myLatticeParams[2] = c;
	myLatticeParams[3] = alpha;
	myLatticeParams[4] = beta;
	myLatticeParams[5] = gamma;
	
	initOrthoAndFracMatrices();
	initRest();
}

template <typename FloatType>
void AbstractFmidCell<FloatType>::init(const typename AbstractFmidCell<FloatType>::Mat33 & orthoMtx)
{
	myOrthoMtx	= orthoMtx;
	myFracMtx	= arma::inv(orthoMtx);

	initLatticeParams();
	initRest();
}

template <typename FloatType>
void AbstractFmidCell<FloatType>::initOrthoAndFracMatrices()
{
	// A - col 0
	myOrthoMtx.fill(0);
	myOrthoMtx.at(0, 0) = myLatticeParams[0];
	// B - col 1
	myOrthoMtx.at(0, 1) = myLatticeParams[1] * cos(Constants::DEG_TO_RAD * myLatticeParams[5]);
	myOrthoMtx.at(1, 1) = myLatticeParams[1] * sin(Constants::DEG_TO_RAD * myLatticeParams[5]);
	// C - col 2
	myOrthoMtx.at(0, 2) = myLatticeParams[2] * cos(Constants::DEG_TO_RAD * myLatticeParams[4]);
	myOrthoMtx.at(1, 2) = myLatticeParams[2] * (cos(Constants::DEG_TO_RAD * myLatticeParams[3]) -
		cos(Constants::DEG_TO_RAD * myLatticeParams[4]) * cos(Constants::DEG_TO_RAD * myLatticeParams[5]));
	myOrthoMtx.at(2, 2) = sqrt(myLatticeParams[2] * myLatticeParams[2] -
		myOrthoMtx(2, 0) * myOrthoMtx(2, 0) -
		myOrthoMtx(2, 1) * myOrthoMtx(2, 1));

	myFracMtx = inv(myOrthoMtx);
}

template <typename FloatType>
void AbstractFmidCell<FloatType>::initLatticeParams()
{
	using namespace arma;

	// Get the lattice vectors
	const typename AbstractFmidCell<FloatType>::Vec3 a = myOrthoMtx.col(0);
	const typename AbstractFmidCell<FloatType>::Vec3 b = myOrthoMtx.col(1);
	const typename AbstractFmidCell<FloatType>::Vec3 c = myOrthoMtx.col(2);

	myLatticeParams[0] = std::sqrt(dot(a, a));
	myLatticeParams[1] = std::sqrt(dot(b, b));
	myLatticeParams[2] = std::sqrt(dot(c, c));
	myLatticeParams[3] = acos(dot(b, c) / (myLatticeParams[1] * myLatticeParams[2])) * Constants::RAD_TO_DEG;
	myLatticeParams[4] = acos(dot(a, c) / (myLatticeParams[0] * myLatticeParams[2])) * Constants::RAD_TO_DEG;
	myLatticeParams[5] = acos(dot(a, b) / (myLatticeParams[0] * myLatticeParams[1])) * Constants::RAD_TO_DEG;
}

template <typename FloatType>
void AbstractFmidCell<FloatType>::initRest()
{
	myVolume = std::fabs(
		dot(myOrthoMtx.col(0),
		cross(myOrthoMtx.col(1), myOrthoMtx.col(2))));
}

template <typename FloatType>
typename AbstractFmidCell<FloatType>::Mat33 AbstractFmidCell<FloatType>::compactNiggli()
{
	using namespace cctbx::uctbx;

	unit_cell * cell = asCctbxUnitCell();

	// Transform this into a reduced cell
	fast_minimum_reduction<FloatType, int> minReduction(*cell);

	// Copy over the reduction to make this a reduced cell
	const scitbx::mat3<int> & reductionMtx = minReduction.r_inv();

	// TODO: Make this work!!

	typename AbstractFmidCell<FloatType>::Mat33 changeOfBasisMtx;
	// Account for:
	// Armadillo	- column-major
	// Cctbx		- row-major
	for(size_t row = 0; row < 3; ++row)
	{
		for(size_t col = 0; col < 3; ++col)
		{
			changeOfBasisMtx.at(row, col) = reductionMtx(row, col);
		}
	}

	delete cell;
	cell = NULL;

	// Finally set the new orthogonalisation matrix
	myOrthoMtx *= changeOfBasisMtx;
	init(myOrthoMtx);

	return changeOfBasisMtx;
}

template <typename FloatType>
FloatType AbstractFmidCell<FloatType>::getLongestVector() const
{
	return std::max(myLatticeParams[0], std::max(myLatticeParams[1], myLatticeParams[2]));	
}

template <typename FloatType>
FloatType AbstractFmidCell<FloatType>::getLongestVectorSq() const
{
	const FloatType longest = getLongestVector();
	return longest * longest;
}

}} // Close the namespace

#endif /* ABSTRACT_FMID_CELL_H */
