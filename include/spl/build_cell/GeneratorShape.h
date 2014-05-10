/*
 * GeneratorShape.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef GENERATOR_SHAPE_H
#define GENERATOR_SHAPE_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#include <vector>

#include <armadillo>

#include "spl/OptionalTypes.h"

// DEFINITION ///////////////////////

namespace spl {
namespace build_cell {
// FORWARD DECLARATIONS ///////

class GeneratorShape
{
public:
  virtual ~GeneratorShape()
  {
  }

  virtual ::arma::vec3
  randomPoint(const ::arma::mat44 * const transform = NULL) const = 0;
  // Generate a random point on an axis that conforms to the shape (if possible).  Axis is assumed to be normalised.
  virtual OptionalArmaVec3
  randomPointOnAxis(const ::arma::vec3 & axis,
      const ::arma::mat44 * const transform = NULL) const = 0;
  // Generate a random point in a plane that conforms to the shape (if possible).
  virtual OptionalArmaVec3
  randomPointInPlane(const ::arma::vec3 & a, const ::arma::vec3 & b,
      const ::arma::mat44 * const transform = NULL) const = 0;

  virtual bool
  isInShape(const ::arma::vec3 & point) const = 0;

  virtual const ::arma::mat44 &
  getTransform() const = 0;
  virtual void
  setTransform(const ::arma::mat44 & transform) = 0;

  virtual UniquePtr< GeneratorShape>::Type
  clone() const = 0;
};

class GeneratorShapeWithShell : public GeneratorShape
{
public:
  GeneratorShapeWithShell();
  GeneratorShapeWithShell(const double shellThickness);

  virtual bool
  isInShell(const ::arma::vec3 & point) const = 0;

  virtual ::arma::vec3
  randomPoint(const ::arma::mat44 * const transform = NULL) const;

  bool
  hasShell() const;
  double
  shellThickness() const;

protected:
  static const double NO_SHELL;

  virtual ::arma::vec3
  doRandomPoint(const ::arma::mat44 * const transform = NULL) const = 0;

private:
  const double myShellThickness;
};

}
}

#endif /* GENERATOR_SHAPE_H */
