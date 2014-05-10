/*
 * Geometry.cpp
 *
 *  Created on: Jan 20, 2014
 *      Author: Martin Uhrin
 */

// INCLUEDES /////////////
#include "spl/math/Geometry.h"

#include "spl/utility/IndexingEnums.h"

namespace spl {
namespace math {

using namespace spl::utility::cart_coords_enum;
using arma::endr;

Line2::Line2(const arma::vec2 & p, const arma::vec2 & q)
{
  // The horizontal and vertical line get myA special treatment
  // in order to make the intersection code robust for doubles
  if(p(Y) == q(Y))
  {
    myA = 0;
    if(q(X) > p(X))
    {
      myB = 1;
      myC = -p(Y);
    }
    else if(q(X) == p(X))
    {
      myB = 0;
      myC = 0;
    }
    else
    {
      myB = -1;
      myC = p(Y);
    }
  }
  else if(q(X) == p(X))
  {
    myB = 0;
    if(q(Y) > p(Y))
    {
      myA = -1;
      myC = p(X);
    }
    else if(q(Y) == p(Y))
    {
      myA = 0;
      myC = 0;
    }
    else
    {
      myA = 1;
      myC = -p(X);
    }
  }
  else
  {
    myA = p(Y) - q(Y);
    myB = q(X) - p(X);
    myC = -p(X) * myA - p(Y) * myB;
  }
}

double
Line2::a() const
{
  return myA;
}

double
Line2::b() const
{
  return myB;
}

double
Line2::c() const
{
  return myC;
}

bool
Line2::isVertical() const
{
  return myA == 0.0;
}

bool
Line2::isHorizontal() const
{
  return myB == 0.0;
}

arma::vec2
intersectionPoint(const Line2 & l1, const Line2 & l2)
{
  const double num1 = l1.b() * l2.c() - l2.b() * l1.c();
  const double num2 = l2.a() * l1.c() - l1.a() * l2.c();
  const double denom = l1.a() * l2.b() - l2.a() * l1.b();
  arma::vec2 pt;
  pt << num1 / denom << endr << num2 / denom;
  return pt;
}

}
}
