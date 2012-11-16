/*
 * SchemaList.cpp
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "factory/SchemaList.h"

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

#include "factory/SchemaDoc.h"
#include "factory/SchemaScalarValueAny.h"

// NAMESPACES ////////////////////////////////

namespace sstbx
{
namespace factory
{

// Instance implementation ///////////////////

const SchemaList & SchemaList::Instance::getSchemaElement() const
{
  return myElement;
}

bool SchemaList::Instance::isRequired() const
{
  return myRequired;
}

int SchemaList::Instance::getSize() const
{
  return mySize;
}

bool SchemaList::Instance::validate(const sstbx::factory::IInputObject &obj, const sstbx::factory::SchemaDoc &doc) const
{
  if(obj.getType() != IInputObject::LIST &&
    obj.getType() != IInputObject::UNDEFINED)
  {
    return false;
  }

  if(getSize() != ISchemaElementInstance::SIZE_UNDEFINED &&
    obj.getSize() != getSize())
  {
    return false;
  }

  IInputObject::SharedPtrConstTyp childObj;

  bool allValid = true;
  BOOST_FOREACH(childObj, obj)
  {
    // Check against all possible list entries
    bool validated = false;
    BOOST_FOREACH(const ChildPtr elementInst, myElement.myChildren)
    {
      validated = elementInst->validate(*childObj, doc);
      if(validated)
        break;
    }
    if(!validated)
    {
      allValid = false;
      break;
    }
  }

  return allValid;
}

ISchemaElementInstance::SharedPtrTyp SchemaList::Instance::clone() const
{
  return SharedPtrTyp(new Instance(*this));
}

size_t SchemaList::Instance::numDefaults() const
{
  return myDefaults.size();
}

const IInputObject::SharedPtrConstTyp SchemaList::Instance::getDefault(const size_t idx) const
{
  IInputObject::SharedPtrConstTyp ptr;

  if(idx < myDefaults.size())
  {
    ptr = myDefaults[idx];
  }

  return ptr;
}

SchemaList::Instance::Instance(const SchemaList & element, const bool required, const int size):
myElement(element),
myRequired(required),
mySize(size)
{}

SchemaList::Instance::Instance(const Instance & toCopy):
myElement(toCopy.getSchemaElement()),
myRequired(toCopy.isRequired()),
mySize(toCopy.getSize())
{
  // TOOD: Implement copying of default values!!
}

// End Instance implementation ///////////////

SchemaList::SchemaList(
    const ::std::string &     name,
    const bool                required,
    const SchemaObjectUid &   uid,
    const int                 size
):
SchemaElement(IInputObject::LIST, name, uid),
myRequired(required),
mySize(size)
{
  insert(SchemaScalarValueAny::create());
}

size_t SchemaList::numChildren() const
{
  return myChildren.size();
}

const SchemaList::ChildPtr SchemaList::operator [](const size_t idx) const
{
  ChildPtr ptr;

  if(idx < numChildren())
  {
    ptr = myChildren[idx];
  }

  return ptr;
}

ISchemaElementInstance::SharedPtrTyp SchemaList::newInstance() const
{
  return ISchemaElementInstance::SharedPtrTyp(new Instance(*this, isRequired(), getSize()));
}

void SchemaList::insert(const ChildPtr & elementInst)
{
  myChildren.push_back(elementInst);
}

//bool SchemaList::validate(const IInputObject & obj, const SchemaDoc & doc) const
//{
//  if(obj.getType() != IInputObject::LIST &&
//    obj.getType() != IInputObject::UNDEFINED)
//  {
//    return false;
//  }
//
//  // Go through all the members of the list and make sure they are valid
//  bool childValid;
//  const IInputObject * childObj = NULL;
//  for(size_t i = 0; i < obj.getSize(); ++i)
//  {
//    childObj = obj[i];
//
//    childValid = false;
//    BOOST_FOREACH(const ::boost::shared_ptr<SchemaObject> & child, myChildren)
//    {
//      childValid |= child->validate(*childObj, doc);
//    }
//    if(!childValid)
//      return false;
//  }
//
//
//
//  return true;
//}

int SchemaList::getSize() const
{
  return mySize;
}

bool SchemaList::isRequired() const
{
  return myRequired;
}

}
}


