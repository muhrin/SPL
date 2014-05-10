/*
 * ConvexHullOutputter.h
 *
 *  Created on: Jul 17, 2013
 *      Author: Martin Uhrin
 */

#ifndef CONVEX_HULL_OUTPUTTER_H
#define CONVEX_HULL_OUTPUTTER_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_WITH_CGAL

// DEFINITION ///////////////////////

namespace spl {

// FORWARD DECLARATIONS ///////

namespace analysis {
class ConvexHull;
class ConvexHullInfoSupplier;

class ConvexHullOutputter
{
public:
  virtual
  ~ConvexHullOutputter()
  {
  }

  virtual bool
  outputHull(const ConvexHull & convexHull) const = 0;
  virtual bool
  outputHull(const ConvexHull & convexHull,
      const ConvexHullInfoSupplier & infoSupplier) const = 0;
};

}
}

#endif // SPL_WITH_CGAL
#endif /* CONVEX_HULL_OUTPUTTER_H */
