/*
 * GenShapeFactory.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "spl/factory/GenShapeFactory.h"

#include "spl/build_cell/GenBox.h"
#include "spl/build_cell/GenCylinder.h"
#include "spl/build_cell/GenSphere.h"
#include "spl/math/Matrix.h"

// NAMESPACES ////////////////////////////////

namespace spl {
namespace factory {

// namespace aliases
namespace ssbc = ::spl::build_cell;
namespace ssf = ::spl::factory;
namespace ssu = ::spl::utility;

bool
GenShapeFactory::createShape(GenShapePtr & shapeOut,
    const builder::GenShape & options) const
{
  if(options.sphere)
    return createSphere(shapeOut, *options.sphere);

  if(options.box)
    return createBox(shapeOut, *options.box);

  if(options.cylinder)
    return createCylinder(shapeOut, *options.cylinder);

  return false;
}

bool
GenShapeFactory::createSphere(GenShapePtr & shapeOut,
    const builder::GenSphere & options) const
{
  UniquePtr< ssbc::GenSphere>::Type sphere(new ssbc::GenSphere(options.radius));
  if(options.pos)
    sphere->setPosition(options.pos->t());

  if(options.shellThickness)
    sphere->setShellThickness(*options.shellThickness);

  shapeOut = sphere;
  return true;
}

bool
GenShapeFactory::createBox(GenShapePtr & shapeOut,
    const builder::GenBox & options) const
{
  ::arma::mat44 transform;
  transform.eye();
  if(options.pos)
    math::setTranslation(transform, options.pos->t());
  if(options.rot)
    math::setRotation(transform, options.rot->t());

  UniquePtr< ssbc::GenBox>::Type box(
      new ssbc::GenBox(options.width, options.height, options.depth, transform));

  if(options.shellThickness)
    box->setShellThickness(*options.shellThickness);

  shapeOut = box;
  return true;
}

bool
GenShapeFactory::createCylinder(GenShapePtr & shapeOut,
    const builder::GenCylinder & options) const
{
  ssbc::GenCylinder::Settings settings;
  settings.shellThickness = options.shellThickness;
  if(options.shellCapped)
    settings.shellCapped = *options.shellCapped;

  UniquePtr< ssbc::GenCylinder>::Type cylinder(
      new ssbc::GenCylinder(options.radius, options.height, settings));

  ::arma::mat44 transform;
  transform.eye();
  if(options.pos)
    math::setTranslation(transform, options.pos->t());
  if(options.rot)
    math::setRotation(transform, options.rot->t());
  cylinder->setTransform(transform);

  shapeOut = cylinder;
  return true;
}

}
}

