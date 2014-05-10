/*
 * Sphere.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef SPHERE_H
#define SPHERE_H

// INCLUDES /////////////////////////////////
#include "spl/SSLib.h"

#include <armadillo>

// FORWARD DECLARES //////////////////////////

namespace spl {
namespace build_cell {

class Sphere
{
public:

  static double radius(const double volume);
  static double volume(const double radius);

  Sphere();
  Sphere(const ::arma::vec3 & position, const double radius = 1);

  double getRadius() const;
  void setRadius(const double radius);

  const ::arma::vec3 & getPosition() const;
  void setPosition(const ::arma::vec3 & position);

  double getVolume() const;
  void setVolume(const double volume);

  ::arma::vec3 randomPoint() const;

private:
  ::arma::vec3 myPosition;
  double myRadius;
  
};

}
}

#endif /* SPHERE_H */
