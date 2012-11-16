/*
 * InputObject.cpp
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "factory/Scalar.h"

#include "factory/IInputObject.h"

// NAMESPACES ////////////////////////////////

namespace sstbx
{
namespace factory
{

Scalar::Scalar(const ::std::string & value):
AbstractInputObject(IInputObject::SCALAR),
myValue(value)
{}

size_t Scalar::getSize() const
{
  return 0;
}

const ::std::string & Scalar::getValue() const
{
  return myValue;
}

// Access mechanism for sequences
const Scalar::InterfacePtrConst Scalar::operator [](const size_t idx) const
{
  return InterfacePtrConst();
}

// Access mechanism for maps
const Scalar::InterfacePtrConst Scalar::operator [](const ::std::string & key) const
{
  return InterfacePtrConst();
}

}
}


