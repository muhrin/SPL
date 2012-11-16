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
namespace sstbx {
namespace build_cell {
class RandomCellDescription;
}
namespace common {
class AbstractFmidCell;
}
}

namespace sstbx
{
namespace build_cell
{

class ICellGenerator
{
public:

	virtual ::sstbx::common::AbstractFmidCell *
    generateCell(const RandomCellDescription & cellDesc) const = 0;
};

}}

#endif /* I_CELL_GENERATOR_H */
