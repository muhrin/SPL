/*
 * GenBox.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef GEN_BOX_H
#define GEN_BOX_H

// INCLUDES /////////////////////////////////
#include "spl/SSLib.h"

#include <boost/optional.hpp>

#include "spl/build_cell/GeneratorShape.h"

// FORWARD DECLARES //////////////////////////

namespace spl {
namespace build_cell {

class GenBox : public GeneratorShape
{
public:
  typedef ::boost::optional< double> ShellThickness;

  GenBox(const double width, const double height, const double depth);
  GenBox(const double width, const double height, const double depth,
      const ::arma::mat44 & transform);
  GenBox(const GenBox & toCopy);

  ::arma::vec3
  getPosition() const;
  void
  setPosition(const ::arma::vec3 & pos);

  const ShellThickness &
  getShellThickness() const;
  void
  setShellThickness(const ShellThickness thickness);

  // From GeneratorShape
  virtual ::arma::vec3
  randomPoint(const ::arma::mat44 * const transform = NULL) const;
  // Generate a random point on an axis that conforms to the shape.  Axis is assumed to be normalised.
  virtual OptionalArmaVec3
  randomPointOnAxis(const ::arma::vec3 & axis,
      const ::arma::mat44 * const transform = NULL) const;
  // Generate a random point in a plane that conforms to the shape.
  virtual OptionalArmaVec3
  randomPointInPlane(const ::arma::vec3 & a, const ::arma::vec3 & b,
      const ::arma::mat44 * const transform = NULL) const;

  virtual bool
  isInShape(const ::arma::vec3 & point) const;

  virtual const ::arma::mat44 &
  getTransform() const;
  virtual void
  setTransform(const ::arma::mat44 & transform);

  virtual UniquePtr< GeneratorShape>::Type
  clone() const;
  // End from GeneratorShape

private:
  bool
  isInShell(const ::arma::vec3 & point) const;
  ::arma::vec3
  randomPoint(const ::arma::mat44 & fullTransform) const;
  void
  randomPoints(::std::vector< ::arma::vec3> & pointsOut, const unsigned int num,
      const ::arma::mat44 & fullTransform) const;
  // Generate a random point on an axis that conforms to the shape.  Axis is assumed to be normalised.
  OptionalArmaVec3
  randomPointOnAxis(const ::arma::vec3 & axis,
      const ::arma::mat44 & fullTransform) const;
  // Generate a random point in a plane that conforms to the shape.
  OptionalArmaVec3
  randomPointInPlane(const ::arma::vec3 & a, const ::arma::vec3 & b,
      const ::arma::mat44 & fullTransform) const;

  const double myWidth, myHalfWidth;
  const double myHeight, myHalfHeight;
  const double myDepth, myHalfDepth;

  ::arma::mat44 myTransform;
  ::arma::mat44 myInvTransform;
  ShellThickness myShellThickness;
};

}
}

#endif /* GEN_BOX_H */
