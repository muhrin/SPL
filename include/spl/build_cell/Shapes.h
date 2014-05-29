/*
 * Sphere.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef SPL__BUILD_CELL__SHAPES_H_
#define SPL__BUILD_CELL__SHAPES_H_

// INCLUDES /////////////////////////////////
#include "spl/SSLib.h"

#include <armadillo>

#include "spl/common/Constants.h"

// FORWARD DECLARES //////////////////////////

namespace spl {
namespace build_cell {
namespace sphere {

inline double
radius(const double volume)
{
  return std::pow(4.0 / 3.0 * volume / common::constants::PI, 1.0 / 3.0);
}
inline double
volume(const double radius)
{
  return 4.0 / 3.0 * common::constants::PI * radius * radius * radius;
}

}
}
}

#endif /* SPL__BUILD_CELL__SHAPES_H_ */
