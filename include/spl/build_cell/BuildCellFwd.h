/*
 * BuildCellFwd.h
 *
 *  Created on: Aug 30, 2012
 *      Author: Martin Uhrin
 */

#ifndef BUILD_CELL_FWD_H
#define BUILD_CELL_FWD_H

// INCLUDES ////////////
#include "spl/SSLib.h"

namespace spl {
namespace build_cell {

// FORWARD DECLARES ////////////
class AtomsDescription;
class AtomsGroup;
class FragmentGenerator;
class StructureGenerator;
class IUnitCellGenerator;
class RandomUnitCellGenerator;
class StructureBuilder;

// TYPEDEFS /////////////////////
typedef UniquePtr<AtomsDescription>::Type AtomsDescriptionPtr;
typedef UniquePtr<AtomsGroup>::Type AtomsGroupPtr;
typedef UniquePtr<FragmentGenerator>::Type IFragmentGeneratorPtr;
typedef UniquePtr<IUnitCellGenerator>::Type IUnitCellGeneratorPtr;
typedef UniquePtr<StructureGenerator>::Type IStructureGeneratorPtr;
typedef UniquePtr<RandomUnitCellGenerator>::Type RandomUnitCellPtr;
typedef UniquePtr<StructureBuilder>::Type StructureBuilderPtr;


}
}

#endif /* BUILD_CELL_FWD_H */
