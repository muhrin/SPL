/*
 * StructureConvexHullInfoSupplier.cpp
 *
 *  Created on: Jul 17, 2013
 *      Author: Martin Uhrin
 */

#include "spl/analysis/StructureConvexHullInfoSupplier.h"

#ifdef SPL_USE_CGAL

#include "spl/common/Structure.h"

namespace spl {
namespace analysis {

void
StructureConvexHullInfoSupplier::addStructure(
    const common::Structure & structure, int id)
{
  myStructures[id] = &structure;
}

::std::string
StructureConvexHullInfoSupplier::getLabel(const ConvexHull & convexHull,
    const ConvexHull::PointId pointId) const
{
  ::std::string label;
  if(pointId == -1)
    return label;

  Structures::const_iterator it = myStructures.find(pointId);
  if(it == myStructures.end())
    return label;

  return it->second->getComposition().toString();
}

}
}

#endif // SPL_USE_CGAL
