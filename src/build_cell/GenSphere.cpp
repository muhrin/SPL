/*
 * GenSphere.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

// INCLUEDES /////////////
#include "spl/build_cell/GenSphere.h"

#include "spl/common/Constants.h"
#include "spl/math/Matrix.h"
#include "spl/math/Random.h"
#include "spl/utility/IndexingEnums.h"

namespace spl {
namespace build_cell {

GenSphere::GenSphere(const double radius) :
    myRadius(::std::abs(radius))
{
  setTransform(::arma::eye< ::arma::mat>(4, 4));
}

GenSphere::GenSphere(const GenSphere & toCopy) :
    myRadius(toCopy.myRadius), myShellThickness(toCopy.myShellThickness)
{
  setTransform(toCopy.myTransform);
}

::arma::vec3
GenSphere::getPosition() const
{
  return math::getTranslation(myTransform);
}

void
GenSphere::setPosition(const ::arma::vec3 & pos)
{
  math::setTranslation(myTransform, pos);
  setTransform(myTransform);
}

const GenSphere::ShellThickness &
GenSphere::getShellThickness() const
{
  return myShellThickness;
}

void
GenSphere::setShellThickness(const ShellThickness thickness)
{
  myShellThickness = thickness;
}

::arma::vec3
GenSphere::randomPoint(const ::arma::mat44 * const transform) const
{
  if(transform)
    return randomPoint(*transform * myTransform);
  else
    return randomPoint(myTransform);
}

OptionalArmaVec3
GenSphere::randomPointOnAxis(const ::arma::vec3 & axis,
    const ::arma::mat44 * const transform) const
{
  if(transform)
    return randomPointOnAxis(axis, *transform * myTransform);
  else
    return randomPointOnAxis(axis, myTransform);
}

OptionalArmaVec3
GenSphere::randomPointInPlane(const ::arma::vec3 & a, const ::arma::vec3 & b,
    const ::arma::mat44 * const transform) const
{
  if(transform)
    return randomPointInPlane(a, b, *transform * myTransform);
  else
    return randomPointInPlane(a, b, myTransform);
}


bool
GenSphere::isInShape(const ::arma::vec3 & point) const
{
  const ::arma::vec3 local = math::transformCopy(point, myInvTransform);
  const double lenSq = ::arma::dot(local, local);
  if(myShellThickness)
  {
    const double delta = lenSq - myRadius;
    if(delta < -*myShellThickness || delta > *myShellThickness)
      return false;
  }
  else
  {
    if(lenSq > myRadius * myRadius)
      return false;
  }
  return true;
}

const ::arma::mat44 &
GenSphere::getTransform() const
{
  return myTransform;
}

void
GenSphere::setTransform(const ::arma::mat44 & transform)
{
  myTransform = transform;
  myInvTransform = math::inverseCopy(transform);
}

UniquePtr< GeneratorShape>::Type
GenSphere::clone() const
{
  return UniquePtr< GeneratorShape>::Type(new GenSphere(*this));
}

double
GenSphere::generateRadius() const
{
  if(myShellThickness)
  {
    const double halfThickness = *myShellThickness * 0.5;
    return myRadius - halfThickness
        + *myShellThickness
            * pow(math::randu< double>(), common::constants::ONE_THIRD);
    //return math::randu(myRadius - halfThickness, myRadius + halfThickness)
//      * pow(math::randu<double>(), common::constants::ONE_THIRD);
  }
  else
    return myRadius * pow(math::randu< double>(), common::constants::ONE_THIRD);
}

::arma::vec3
GenSphere::randomPoint(const ::arma::mat44 & fullTransform) const
{
  using namespace utility::cart_coords_enum;

  // Get a random point with normally distributed x, y and z with with 0 mean and 1 variance.
  ::arma::vec3 point;
  point.randn();

  // Normalise and scale
  point *= generateRadius() / (sqrt(::arma::dot(point, point)));

  // Transform it
  math::transform(point, myTransform);
  return point;
}

OptionalArmaVec3
GenSphere::randomPointOnAxis(const ::arma::vec3 & axis,
    const ::arma::mat44 & fullTransform) const
{
  // TODO: Axis/sphere intersection test
  return ::arma::vec3(generateRadius() * axis);
}

OptionalArmaVec3
GenSphere::randomPointInPlane(const ::arma::vec3 & a, const ::arma::vec3 & b,
    const ::arma::mat44 & fullTransform) const
{
  // TODO: Do proper stats for a 2d circle
  // TODO: Plane/sphere intersection test
  const ::arma::vec3 point(
      math::randn< double>() * a + math::randn< double>() * b);
  // Normalise, scale and translate the point
  return OptionalArmaVec3(
      point * generateRadius() / (sqrt(::arma::dot(point, point))));
}

}
}
