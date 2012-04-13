/*
 * NonOrthorhombicUnitCell.h
 *
 * 
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef NON_ORTHORHOMBIC_UNIT_CELL_H
#define NON_ORTHORHOMBIC_UNIT_CELL_H

// INCLUDES /////////////////////////////////////////////
//#include <cctbx/uctbx/fast_minimum_reduction.h>
//#include <scitbx/vec3.h>

#include "AbstractFmidCell.h"
#include "Utils.h"

// FORWARD DECLARATIONS ////////////////////////////////////


namespace sstbx { namespace common {

template<typename FloatType = double>
class NonOrthorhombicUnitCell : public AbstractFmidCell<FloatType>
{
public:

	NonOrthorhombicUnitCell();

	explicit NonOrthorhombicUnitCell(
		const FloatType a, const FloatType b, const FloatType c,
		const FloatType alpha, const FloatType beta, const FloatType gamma):
	AbstractFmidCell<FloatType>(a, b, c, alpha, beta, gamma)
	{
		/*using namespace cctbx::uctbx;

		unit_cell * cell = ABSTRACT_FMID_CELL::asCctbxUnitCell();

		// Transform this into a reduced cell
		fast_minimum_reduction<FloatType, int> minReduction(*cell);

		// Copy over the reduction to make this a reduced cell
		const scitbx::mat3<int> & reductionMtx = minReduction.r_inv();

		ABSTRACT_FMID_CELL_TYPE::Mat33 changeOfBasisMtx;
		for(size_t row = 0; row < 3; ++row)
		{
			for(size_t col = 0; col < 3; ++col)
			{
				// Account for:
				// Armadillo - column-major
				// Cctbx - row-major
				changeOfBasisMtx.at(row, col) = reductionMtx(col, row);
			}
		}

		delete cell;
		cell = NULL;

		ABSTRACT_FMID_CELL::myOrthoMtx *= changeOfBasisMtx;
		init(ABSTRACT_FMID_CELL::myOrthoMtx);*/
	}


};

}}

#endif /* NON_ORTHORHOMBIC_UNIT_CELL_H */
