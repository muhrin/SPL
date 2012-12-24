/*
 * UniqueStructureSet.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: muhrin
 */

// INCLUDES /////////////////////////////////////
#include "utility/UniqueStructureSet.h"


namespace sstbx {
namespace utility {

UniqueStructureSet<common::Structure *>::UniqueStructureSet(const IStructureComparator & comparator):
detail::UniqueStructureSetBase<common::Structure *>(comparator)
{}

UniqueStructureSet<common::Structure *>::insert_return_type
UniqueStructureSet<common::Structure *>::insert(common::Structure * structure)
{
  MapInsertReturn pair = insertStructure(structure, *structure);
  return insert_return_type(iterator(pair.first), pair.second);
}

<<<<<<< HEAD
=======

>>>>>>> d58c64683591480856d9a864079efb24c6a736a8
}
}
