/*
 * OrthorhombicUnitCell.h
 *
 * 
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef ORTHORHOMBIC_UNIT_CELL_H
#define ORTHORHOMBIC_UNIT_CELL_H

// INCLUDES /////////////////////////////////////////////
#include "AbstractFmidCell.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx { namespace common {

template<typename FloatType = double>
class OrthorhombicUnitCell : public AbstractFmidCell<FloatType>
{
public:
	explicit OrthorhombicUnitCell(
		const FloatType a, const FloatType b, const FloatType c,
		const FloatType alpha, const FloatType beta, const FloatType gamma):
	AbstractFmidCell<FloatType>(a, b, c, alpha, beta, gamma)
	{}

	virtual FloatType getDistanceSqMinimumImg(
		ABSTRACT_FMID_CELL_TYPE::Vec3 const & a, ABSTRACT_FMID_CELL_TYPE::Vec3 const & b) const
	{
		ABSTRACT_FMID_CELL_TYPE::Vec3 r = b - a;
		// Fractionalise and wrap to maximum 0.5 in any direction
		fractionaliseInplace(r);
		r(0) = fmod(r(0), 0.5);
		r(1) = fmod(r(1), 0.5);
		r(2) = fmod(r(2), 0.5);
		// Orthogonalise back
		orthogonaliseInplace(r);

		return arma::dot(r, r);
	}

	virtual ABSTRACT_FMID_CELL_TYPE::Vec3 getVecMinimumImg(
		ABSTRACT_FMID_CELL_TYPE::Vec3 const & a, ABSTRACT_FMID_CELL_TYPE::Vec3 const & b) const
	{
		ABSTRACT_FMID_CELL_TYPE::Vec3 r = b - a;
		// Fractionalise and wrap to maximum 0.5 in any direction
		fractionaliseInplace(r);
		r(0) = fmod(r(0), 0.5);
		r(1) = fmod(r(1), 0.5);
		r(2) = fmod(r(2), 0.5);
		// Orthogonalise back
		orthogonaliseInplace(r);

		return r;
	}

};

}}

#endif /* ORTHORHOMBIC_UNIT_CELL_H */
