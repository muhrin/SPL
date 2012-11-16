/*
 * SchemaScalarValueLiteral.cpp
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "factory/SchemaScalarValueLiteral.h"

#include "factory/IInputObject.h"

// NAMESPACES ////////////////////////////////

namespace sstbx
{
namespace factory
{

::boost::shared_ptr<SchemaScalarValueLiteral>
SchemaScalarValueLiteral::newInstance(const ::std::string & name)
{
  return ::boost::shared_ptr<SchemaScalarValueLiteral>(new SchemaScalarValueLiteral(name));
}

SchemaScalarValueLiteral::SchemaScalarValueLiteral(const std::string &name):
SchemaScalarValue(name)
{}

const SchemaElement & SchemaScalarValueLiteral::getSchemaElement() const
{
  // Scalar literals values are their own elements
  return *this;
}

bool SchemaScalarValueLiteral::isRequired() const
{
  return false;
}

int SchemaScalarValueLiteral::getSize() const
{
  return 0;
}

bool SchemaScalarValueLiteral::validate(const IInputObject & obj, const SchemaDoc & doc) const
{
  if(obj.getType() != IInputObject::SCALAR &&
    obj.getType() != IInputObject::UNDEFINED)
  {
    return false;
  }

  if(obj.getSize() != 0)
  {
    return false;
  }

  if(obj.getValue() != getName())
  {
    return false;
  }

  return true;
}


size_t SchemaScalarValueLiteral::numDefaults() const
{
  // Can't have a 'default value' - doesn't make sense
  return 0;
}

const IInputObject::SharedPtrConstTyp SchemaScalarValueLiteral::getDefault(const size_t idx) const
{
  // Can't have a 'default value' - doesn't make sense
  return IInputObject::SharedPtrConstTyp();
}

size_t SchemaScalarValueLiteral::numChildren() const
{
  // Can't have children
  return 0;
}

const SchemaScalarValueLiteral::ChildPtr SchemaScalarValueLiteral::operator[](const size_t idx) const
{
  return ChildPtr();
}

::boost::shared_ptr<ISchemaElementInstance> SchemaScalarValueLiteral::newInstance() const
{
  return ::boost::shared_ptr<ISchemaElementInstance>(new SchemaScalarValueLiteral(*this));
}

}
}


