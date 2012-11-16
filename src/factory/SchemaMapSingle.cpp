/*
 * SchemaMapSingle.cpp
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "factory/SchemaMapSingle.h"

#include <boost/foreach.hpp>

#include "factory/SchemaScalarValueAny.h"
#include "factory/SchemaScalarValueLiteral.h"


// NAMESPACES ////////////////////////////////

namespace sstbx
{
namespace factory
{


// INSTANCE IMPLEMENTATION ///////////////////

void SchemaMapSingle::Instance::setRequired(const bool required)
{
  myRequired = required;
}

void SchemaMapSingle::Instance::setDefault(const DefaultValueConstPointer & defaultVal)
{
  // TODO: Validate default being set somehow...
  myDefault = defaultVal;
}

const SchemaMapSingle & SchemaMapSingle::Instance::getSchemaElement() const
{
  return myElement;
}

bool SchemaMapSingle::Instance::isRequired() const
{
  return myRequired;
}

int SchemaMapSingle::Instance::getSize() const
{
  // Scalar is always size 1
  return 1;
}

bool SchemaMapSingle::Instance::validate(const sstbx::factory::IInputObject &obj, const sstbx::factory::SchemaDoc &doc) const
{
  if(obj.getType() != IInputObject::SCALAR &&
    obj.getType() != IInputObject::UNDEFINED)
  {
    return false;
  }

  if(obj.getSize() != 1)
  {
    return false;
  }

  const IInputObject::SharedPtrConstTyp child = obj[0];
  
  bool valid = false;
  BOOST_FOREACH(const ChildValuePtr & childObj, myElement.getChildren())
  {
    if(childObj->validate(obj, doc))
    {
      valid = true;
      break;
    }
  }

  return valid;
}

size_t SchemaMapSingle::Instance::numDefaults() const
{
  return myDefault.get() ? 1 : 0;
}

const SchemaMapSingle::DefaultValueConstPointer
SchemaMapSingle::Instance::getDefault(const size_t idx) const
{
  if(idx == 0)
    return myDefault;

  return DefaultValueConstPointer();
}

SchemaMapSingle::Instance::Instance(
  const SchemaMapSingle & element,
  const bool required,
  const DefaultValueConstPointer & _default):
myElement(element),
myRequired(required),
myDefault(_default)
{}

SchemaMapSingle::Instance::Instance(const Instance & toCopy):
myElement(toCopy.myElement),
myRequired(toCopy.isRequired()),
myDefault(toCopy.myDefault)
{

}

// END INSTANCE IMPLEMENTATION //////////////////
  
SchemaMapSingle::SchemaMapSingle(
  const ::std::string &                   name,
  const bool                              required,
  const SchemaObjectUid &                 uid,
  const DefaultValueConstPointer &        _default,
  const SchemaObjectUid                   supertype
):
SchemaElement(IInputObject::MAP, name, uid, false /*TODO: Put in isAbstract*/, supertype),
myRequired(required),
myDefault(_default),
myFirstChildInserted(false)
{
  // By default a scalar can have any value, if a child value is added after
  // instantiation then this entry is automatically removed
  insert(SchemaScalarValueAny::create());
}

size_t SchemaMapSingle::numChildren() const
{
  return myChildren.size();
}

const ISchemaElementInstance::SharedPtrTyp SchemaMapSingle::operator[](const size_t idx) const
{
  ISchemaElementInstance::SharedPtrTyp child;

  if(idx < numChildren())
  {
    child = myChildren[idx];
  }

  return child;
}


::boost::shared_ptr<ISchemaElementInstance> SchemaMapSingle::newInstance() const
{
  // TODO: Set required value from element local value
  return ::boost::shared_ptr<ISchemaElementInstance>(new Instance(*this, isRequired(), myDefault));
}

void SchemaMapSingle::insert(const ChildValuePtr & scalarValue)
{
  // Have any child values been inserted yet?
  if(myFirstChildInserted)
  {
    myChildren.erase(myChildren.begin());
    myFirstChildInserted = true;
  }
  // TODO: Check if we already have this value!
  myChildren.push_back(scalarValue);
}

void SchemaMapSingle::insert(const ::std::string & name)
{
  insert(SchemaScalarValueLiteral::newInstance(name));
}

const SchemaMapSingle::ChildrenContainer & SchemaMapSingle::getChildren() const
{
  return myChildren;
}

bool SchemaMapSingle::isRequired() const
{
  return myRequired;
}

const SchemaMapSingle::DefaultValueConstPointer SchemaMapSingle::getDefault() const
{
  return myDefault;
}

}
}


