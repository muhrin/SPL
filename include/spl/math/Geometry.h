/*
 * Geometry.h
 *
 *  Created on: Jan 20, 2014
 *      Author: Martin Uhrin
 */

#ifndef GEOMETRY_H
#define GEOMETRY_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#include <armadillo>

// DEFINITION ///////////////////////

namespace spl {
namespace math {

// Line, represented as coefficients in equation:
//
// l: ax + by + c = 0
//
class Line2
{
public:
  Line2(const arma::vec2 & p, const arma::vec2 & q);

  double
  a() const;
  double
  b() const;
  double
  c() const;

  bool
  isVertical() const;
  bool
  isHorizontal() const;

private:
  double myA, myB, myC;
};

arma::vec2
intersectionPoint(const Line2 & l1, const Line2 & l2);

}
}

#endif /* MATRIX_H */
