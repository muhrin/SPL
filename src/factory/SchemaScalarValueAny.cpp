/*
 * SchemaScalarValueAny.cpp
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "factory/SchemaScalarValueAny.h"

#include "factory/IInputObject.h"

// NAMESPACES ////////////////////////////////

namespace sstbx
{
namespace factory
{

::boost::shared_ptr<SchemaScalarValueAny>
SchemaScalarValueAny::create()
{
  return ::boost::shared_ptr<SchemaScalarValueAny>(new SchemaScalarValueAny());
}

SchemaScalarValueAny::SchemaScalarValueAny():
SchemaScalarValue("[ANY]")
{}

const SchemaElement & SchemaScalarValueAny::getSchemaElement() const
{
  // Scalar literals values are their own elements
  return *this;
}

bool SchemaScalarValueAny::isRequired() const
{
  return false;
}

int SchemaScalarValueAny::getSize() const
{
  return 0;
}

bool SchemaScalarValueAny::validate(const IInputObject & obj, const SchemaDoc & doc) const
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

  return true;
}


size_t SchemaScalarValueAny::numDefaults() const
{
  // Can't have a 'default value' - doesn't make sense
  return 0;
}

const IInputObject::SharedPtrConstTyp SchemaScalarValueAny::getDefault(const size_t idx) const
{
  // Can't have a default value, return NULL
  return IInputObject::SharedPtrConstTyp();
}

size_t SchemaScalarValueAny::numChildren() const
{
  // Can't have children
  return 0;
}

const SchemaScalarValueAny::ChildPtr SchemaScalarValueAny::operator[](const size_t idx) const
{
  return ChildPtr();
}

::boost::shared_ptr<ISchemaElementInstance> SchemaScalarValueAny::newInstance() const
{
  return ::boost::shared_ptr<ISchemaElementInstance>(new SchemaScalarValueAny());
}

}
}


