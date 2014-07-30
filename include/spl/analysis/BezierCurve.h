/*
 * BezierCurve.h
 *
 *  Created on: Jun 25, 2014
 *      Author: Martin Uhrin
 */

#ifndef SPL__ANALYSIS__BEZIER_CURVE_H_
#define SPL__ANALYSIS__BEZIER_CURVE_H_

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_USE_CGAL

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {

template< typename K>
class BezierCurve
{
public:
  void
  reverse()
  {
    std::reverse(control, control + 4);
  }

  typename K::FT alpha;
  typename K::FT beta;
  typename K::Point_2 control[4];
};

}
}

#endif // SPL_USE_CGAL
#endif /* SPL__ANALYSIS__BEZIER_CURVE_H_ */
