/*
 * Sphere.h
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */


// INCLUDES /////////////////
#include "spl/build_cell/Sphere.h"

#include "spl/common/Constants.h"

namespace spl {
namespace build_cell {

double Sphere::radius(const double volume)
{
  return std::pow(
    common::constants::THREE_FOURTHS * volume / common::constants::PI,
    common::constants::ONE_THIRD
  );
}

double Sphere::volume(const double r)
{
  return common::constants::FOUR_THIRDS * common::constants::PI * r * r * r;
}

Sphere::Sphere():
myRadius(0.0)
{
  myPosition.zeros();
}

Sphere::Sphere(const arma::vec3 & position, const double radius):
myPosition(position),
myRadius(radius)
{
}

double Sphere::getRadius() const
{
  return myRadius;
}

void Sphere::setRadius(const double radius)
{
  myRadius = radius;
}

const ::arma::vec3 & Sphere::getPosition() const
{
  return myPosition;
}

void Sphere::setPosition(const arma::vec3 & position)
{
  myPosition = position;
}

double Sphere::getVolume() const
{
  return volume(myRadius);
}

void Sphere::setVolume(const double volume)
{
  myRadius = radius(volume);
}

::arma::vec3 Sphere::randomPoint() const
{
  ::arma::vec3 pt;
  pt.randu();
  pt *= myRadius;
  pt += myPosition;
  return pt;
}



}
}
