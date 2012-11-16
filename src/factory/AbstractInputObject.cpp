/*
 * AbstractInputObject.cpp
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "factory/AbstractInputObject.h"

// NAMESPACES ////////////////////////////////

namespace sstbx
{
namespace factory
{

AbstractInputObject::AbstractInputObject(const IInputObject::Type        type):
myType(type)
{}

IInputObject::Type AbstractInputObject::getType() const
{
  return myType;
}

}
}


