/*
 * GenCylinder.cpp
 *
 *  Created on: Nov 24, 2013
 *      Author: Martin Uhrin
 */

// INCLUEDES /////////////
#include "spl/build_cell/GenCylinder.h"

#include "spl/common/Constants.h"
#include "spl/math/Matrix.h"
#include "spl/math/Random.h"
#include "spl/utility/IndexingEnums.h"

namespace spl {
namespace build_cell {

GenCylinder::GenCylinder(const double radius, const double height) :
    myRadius(radius), myHalfHeight(0.5 * height)
{
  setTransform(::arma::eye(4, 4));
}

GenCylinder::GenCylinder(const double radius, const double height,
    const Settings & settings) :
    GeneratorShapeWithShell(
        settings.shellThickness ?
            *settings.shellThickness : GeneratorShapeWithShell::NO_SHELL), myRadius(
        radius), myHalfHeight(0.5 * height), mySettings(settings)
{
  setTransform(::arma::eye(4, 4));
}

GenCylinder::GenCylinder(const GenCylinder & toCopy) :
    myRadius(toCopy.myRadius), myHalfHeight(toCopy.myHalfHeight), mySettings(
        toCopy.mySettings), myTransform(toCopy.myTransform),
        myInvTransform(toCopy.myInvTransform)
{
}

::arma::vec3
GenCylinder::getPosition() const
{
  return math::getTranslation(myTransform);
}

void
GenCylinder::setPosition(const ::arma::vec3 & pos)
{
  math::setTranslation(myTransform, pos);
  setTransform(myTransform);
}

OptionalArmaVec3
GenCylinder::randomPointOnAxis(const ::arma::vec3 & axis,
    const ::arma::mat44 * const transform) const
{
  SSLIB_DIE_NOT_IMPLEMENTED();
  return OptionalArmaVec3();
}

OptionalArmaVec3
GenCylinder::randomPointInPlane(const ::arma::vec3 & a, const ::arma::vec3 & b,
    const ::arma::mat44 * const transform) const
{
  SSLIB_DIE_NOT_IMPLEMENTED();
  return OptionalArmaVec3();
}

bool
GenCylinder::isInShape(const ::arma::vec3 & point) const
{
  using namespace utility::cart_coords_enum;

  if(hasShell())
    return isInShell(point);
  else
  {
    // Transform to local coordinates
    const ::arma::vec3 local = math::transformCopy(point, myInvTransform);

    if(local(Z) < -myHalfHeight ||
        local(Z) > myHalfHeight)
      return false;

    const double rSqDelta = local(X) * local(X) + local(Y) * local(Y) -
        myRadius * myRadius;

    if(rSqDelta > 0.0)
      return false;

    return true;
  }
}

const ::arma::mat44 &
GenCylinder::getTransform() const
{
  return myTransform;
}

void
GenCylinder::setTransform(const ::arma::mat44 & transform)
{
  myTransform = transform;
  myInvTransform = math::inverseCopy(myTransform);
}

UniquePtr< GeneratorShape>::Type
GenCylinder::clone() const
{
  return UniquePtr< GeneratorShape>::Type(new GenCylinder(*this));
}

bool
GenCylinder::isInShell(const ::arma::vec3 & point) const
{
  using namespace utility::cart_coords_enum;

  SSLIB_ASSERT(hasShell());

  // Transform to local coordinates
  const ::arma::vec3 local = math::transformCopy(point, myInvTransform);
  const double halfThickness = 0.5 * shellThickness();

  if(local(Z) < (-myHalfHeight - halfThickness) ||
      local(Z) > (myHalfHeight + halfThickness))
    return false;

  const double rDelta = ::std::sqrt(local(X) * local(X) + local(Y) * local(Y)) -
      myRadius;

  if(rDelta > halfThickness)
    return false;

  if(mySettings.shellCapped)
  {
    if(local(Z) > (-myHalfHeight + halfThickness) &&
        local(Z) < (myHalfHeight - halfThickness) &&
        rDelta < -halfThickness)
      return false;
  }
  else if(rDelta < -halfThickness)
    return false; // Not capped and on inside of cylinder

  return true;
}

::arma::vec3
GenCylinder::doRandomPoint(const ::arma::mat44 * const transform) const
{
  using namespace utility::cart_coords_enum;

  const double theta = math::randu(0.0, common::constants::TWO_PI);
  const double r = ::std::sqrt(math::randu< double>()) * myRadius;

  ::arma::vec3 pt;
  pt(X) = r * ::std::cos(theta);
  pt(Y) = r * ::std::sin(theta);
  pt(Z) = math::randu(-myHalfHeight, myHalfHeight);

  math::transform(pt, myTransform);
  return pt;
}

}
}
