/*
 * IConvexHullInfoSupplier.h
 *
 *  Created on: Jul 17, 2013
 *      Author: Martin Uhrin
 */

#ifndef CONVEX_HULL_INFO_SUPPLIER_H
#define CONVEX_HULL_INFO_SUPPLIER_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_USE_CGAL

#include "spl/analysis/ConvexHull.h"

// DEFINITION ///////////////////////

namespace spl {
// FORWARD DECLARATIONS ///////

namespace analysis {

class ConvexHullInfoSupplier
{
public:
  virtual
  ~ConvexHullInfoSupplier()
  {
  }

  virtual ::std::string
  getLabel(const ConvexHull & convexHull,
      const ConvexHull::PointId pointId) const = 0;
};

}
}

#endif // SPL_USE_CGAL
#endif /* CONVEX_HULL_INFO_SUPPLIER_H */
