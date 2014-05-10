/*
 * GenCylinder.h
 *
 *  Created on: Nov 24, 2013
 *      Author: Martin Uhrin
 */

#ifndef GEN_CYLINDER_H
#define GEN_CYLINDER_H

// INCLUDES /////////////////////////////////
#include "spl/SSLib.h"

#include <boost/optional.hpp>

#include "spl/build_cell/GeneratorShape.h"

// FORWARD DECLARES //////////////////////////

namespace spl {
namespace build_cell {

class GenCylinder : public GeneratorShapeWithShell
{
public:
  typedef ::boost::optional< double> ShellThickness;

  struct Settings
  {
    Settings() :
        shellCapped(false)
    {
    }
    ShellThickness shellThickness;
    bool shellCapped;
  };

  GenCylinder(const double radius, const double height);
  GenCylinder(const double radius, const double height,
      const Settings & settings);
  GenCylinder(const GenCylinder & toCopy);
  virtual
  ~GenCylinder()
  {
  }

  ::arma::vec3
  getPosition() const;
  void
  setPosition(const ::arma::vec3 & pos);

  // From GeneratorShape
  virtual OptionalArmaVec3
  randomPointOnAxis(const ::arma::vec3 & axis,
      const ::arma::mat44 * const transform = NULL) const;
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

  virtual bool
  isInShell(const ::arma::vec3 & pos) const;

private:
  virtual ::arma::vec3
  doRandomPoint(const ::arma::mat44 * const transform = NULL) const;

  const double myRadius;
  const double myHalfHeight;
  const Settings mySettings;
  ::arma::mat44 myTransform;
  ::arma::mat44 myInvTransform;
};

}
}

#endif /* GEN_CYLINDER_H */
