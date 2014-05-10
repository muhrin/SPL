/*
 * GenShapeFactory.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef GEN_SHAPE_FACTORY_H
#define GEN_SHAPE_FACTORY_H

// INCLUDES /////////////////////////////////////////////
#include "spl/SSLib.h"

#include "spl/build_cell/GeneratorShape.h"
#include "spl/factory/SsLibYamlSchema.h"

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace factory {

class GenShapeFactory : ::boost::noncopyable
{
public:
  typedef UniquePtr< build_cell::GeneratorShape>::Type GenShapePtr;

  bool
  createShape(GenShapePtr & shapeOut, const builder::GenShape & options) const;
  bool
  createBox(GenShapePtr & shapeOut, const builder::GenBox & options) const;
  bool
  createCylinder(GenShapePtr & shapeOut,
      const builder::GenCylinder & options) const;
  bool
  createSphere(GenShapePtr & shapeOut,
      const builder::GenSphere & options) const;
};

}
}

#endif /* GEN_SHAPE_FACTORY_H */

