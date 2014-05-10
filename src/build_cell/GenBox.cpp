/*
 * GenBox.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

// INCLUEDES /////////////
#include "spl/build_cell/GenBox.h"

#include "spl/math/Matrix.h"
#include "spl/math/Random.h"
#include "spl/utility/IndexingEnums.h"

namespace spl {
namespace build_cell {

GenBox::GenBox(const double width, const double height, const double depth):
    myWidth(width), myHalfWidth(0.5 * width), myHeight(height),
    myHalfHeight(0.5 * height), myDepth(depth), myHalfDepth(0.5 * depth)
{
  setTransform(::arma::eye< ::arma::mat>(4, 4));
}

GenBox::GenBox(const double width, const double height, const double depth,
    const ::arma::mat44 & transform) :
    myWidth(width), myHalfWidth(0.5 * width), myHeight(height),
    myHalfHeight(0.5 * height), myDepth(depth), myHalfDepth(0.5 * depth),
    myTransform(transform)
{
  setTransform(transform);
}

GenBox::GenBox(const GenBox & toCopy) :
    myWidth(toCopy.myWidth), myHalfWidth(toCopy.myHalfWidth),
    myHeight(toCopy.myHeight), myHalfHeight(toCopy.myHalfHeight),
    myDepth(toCopy.myDepth), myHalfDepth(toCopy.myHalfDepth),
    myTransform(toCopy.myTransform), myShellThickness(toCopy.myShellThickness)
{
}

::arma::vec3
GenBox::getPosition() const
{
  return myTransform.col(3).rows(0, 2);
}

void
GenBox::setPosition(const ::arma::vec3 & pos)
{
  myTransform.col(3).rows(0, 2) = pos;
}

const GenBox::ShellThickness &
GenBox::getShellThickness() const
{
  return myShellThickness;
}

void
GenBox::setShellThickness(const ShellThickness thickness)
{
  myShellThickness = thickness;
}

::arma::vec3
GenBox::randomPoint(const ::arma::mat44 * const transform) const
{
  if(transform)
    return randomPoint(*transform * myTransform);
  else
    return randomPoint(myTransform);
}

OptionalArmaVec3
GenBox::randomPointOnAxis(const ::arma::vec3 & axis,
    const ::arma::mat44 * const transform) const
{
  if(transform)
    return randomPointOnAxis(axis, *transform * myTransform);
  else
    return randomPointOnAxis(axis, myTransform);
}

OptionalArmaVec3
GenBox::randomPointInPlane(const ::arma::vec3 & a, const ::arma::vec3 & b,
    const ::arma::mat44 * const transform) const
{
  if(transform)
    return randomPointInPlane(a, b, *transform * myTransform);
  else
    return randomPointInPlane(a, b, myTransform);
}

bool
GenBox::isInShape(const ::arma::vec3 & point) const
{
  using namespace utility::cart_coords_enum;

  const ::arma::vec3 local = math::transformCopy(point, myInvTransform);
  if(local(X) < -myHalfWidth || local(X) > myHalfWidth)
    return false;
  if(local(Y) < -myHalfHeight|| local(Y) > myHalfHeight)
    return false;
  if(local(Z) < -myHalfDepth || local(Z) > myHalfDepth)
    return false;

  if(myShellThickness != 0.0 && !isInShell(local))
    return false;

  return true;
}

const ::arma::mat44 &
GenBox::getTransform() const
{
  return myTransform;
}

void
GenBox::setTransform(const ::arma::mat44 & transform)
{
  myTransform = transform;
  myInvTransform = math::inverseCopy(myTransform);
}

bool
GenBox::isInShell(const ::arma::vec3 & point) const
{
  using namespace utility::cart_coords_enum;

  if(point(X) > myHalfWidth)
  {
    // Could be at the right size or the top
    if(point(X) > myHalfWidth + myHeight)
    {
      if(point(Z) > -myHalfDepth && point(X) < myHalfDepth)
      {
        // It's on the top

      }
    }
  }
  else if(point(X) < myHalfWidth)
  {
    if(point(Z) < -myHalfDepth || point(Z) > myHalfDepth)
      return false;
    else
      return true;
  }
  return false;
}

UniquePtr< GeneratorShape>::Type
GenBox::clone() const
{
  return UniquePtr< GeneratorShape>::Type(new GenBox(*this));
}

// To get the shell use rejection algorithm based on flattening a box of a certain
// thickness and finding points on that:
//    -
//   |s|
//  - - - -
// |s|b|s|t|
//  - - - -
//   |s|
//    -
::arma::vec3
GenBox::randomPoint(const ::arma::mat44 & fullTransform) const
{
  using namespace utility::cart_coords_enum;

  ::arma::vec3 point;
  // TODO: Finish this
  //if(myShellThickness)
  //{
  //  point(Y) = math::rand<double>(*myShellThickness);
  //  do
  //  {
  //    point(X) = math::randu(-myHalfWidth - myHeight, myHalfWidth + myHeight + myWidth);
  //    point(Z) = math::randu(-myHalfDepth - myHeight, myHalfDepth + myHeight);
  //  } while(!isInShell(point))
  //}
  //else // Standard box
  {
    point(X) = math::randu(-myHalfWidth, myHalfWidth);
    point(Y) = math::randu(-myHalfHeight, myHalfHeight);
    point(Z) = math::randu(-myHalfDepth, myHalfDepth);
  }
  // Transform
  math::transform(point, fullTransform);
  return point;
}

void
GenBox::randomPoints(::std::vector< ::arma::vec3> & pointsOut,
    const unsigned int num, const ::arma::mat44 & fullTransform) const
{
  for(size_t i = 0; i < num; ++i)
    pointsOut.push_back(randomPoint(fullTransform));
}

OptionalArmaVec3
GenBox::randomPointOnAxis(const ::arma::vec3 & axis,
    const ::arma::mat44 & fullTransform) const
{
  // TODO: Intersection test of line and box
  const ::arma::vec3 point(randomPoint(fullTransform));
  return OptionalArmaVec3(::arma::dot(point, axis) * axis);
}

OptionalArmaVec3
GenBox::randomPointInPlane(const ::arma::vec3 & a, const ::arma::vec3 & b,
    const ::arma::mat44 & fullTransform) const
{
  // TODO: Intersection test of plane and box
  const ::arma::vec3 normal(::arma::cross(a, b)), point(
      randomPoint(fullTransform));
  return OptionalArmaVec3(point - ::arma::dot(normal, point) * normal);
}

}
}
