/*
 * StructureConvexHullInfoSupplier.h
 *
 *  Created on: Jul 17, 2013
 *      Author: Martin Uhrin
 */

#ifndef STRUCTURE_CONVEX_HULL_INFO_SUPPLIER_H
#define STRUCTURE_CONVEX_HULL_INFO_SUPPLIER_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_USE_CGAL

#include <map>

#include "spl/analysis/ConvexHull.h"
#include "spl/analysis/ConvexHullInfoSupplier.h"

// DEFINITION ///////////////////////

namespace spl {

// FORWARD DECLARATIONS ///////
namespace common {
class Structure;
}

namespace analysis {

class StructureConvexHullInfoSupplier : public ConvexHullInfoSupplier
{
public:
  void
  addStructure(const common::Structure & structure,
      const ConvexHull::PointId id);
  virtual ::std::string
  getLabel(const ConvexHull & convexHull,
      const ConvexHull::PointId pointId) const;

private:
  typedef ::std::map< ConvexHull::PointId, const common::Structure *> Structures;

  Structures myStructures;
};

}
}

#endif // SPL_USE_CGAL

#endif /* STRUCTURE_CONVEX_HULL_INFO_SUPPLIER_H */
