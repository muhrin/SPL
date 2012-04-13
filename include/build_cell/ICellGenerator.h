/*
 * ICellGenerator.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef I_CELL_GENERATOR_H
#define I_CELL_GENERATOR_H

// INCLUDES ////////////

// FORWARD DECLARATIONS ///////
namespace sstbx { namespace common {
	template <typename FloatType>
	class AbstractFmidCell;
}}

namespace sstbx { namespace build_cell {

template <typename FloatType>
class ICellGenerator
{
public:

	virtual ::sstbx::common::AbstractFmidCell<FloatType> * generateCell() const = 0;
};

}}

#endif /* I_CELL_GENERATOR_H */
