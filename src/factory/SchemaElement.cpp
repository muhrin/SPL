/*
 * SchemaElement.cpp
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "factory/SchemaElement.h"

// NAMESPACES ////////////////////////////////

namespace sstbx
{
namespace factory
{

const SchemaObjectUid SchemaElement::NULL_SUPERTYPE  = "_NULL_PARENT";
const SchemaObjectUid SchemaElement::UID_ANONYMOUS   = "_ANONYMOUS";

SchemaElement::SchemaElement(
  const sstbx::factory::IInputObject::Type type,
  const std::string &                      name,
  const sstbx::factory::SchemaObjectUid &  uid,
  const bool                               isAbstract,
  const SchemaObjectUid                    supertype):
myType(type),
myName(name),
myUid(uid),
myAbstract(isAbstract),
mySupertype(supertype)
{}

const SchemaObjectUid & SchemaElement::getUid() const
{
  return myUid;
}

const ::std::string & SchemaElement::getName() const
{
  return myName;
}

IInputObject::Type SchemaElement::getType() const
{
  return myType;
}

const SchemaObjectUid & SchemaElement::getSupertype() const
{
  return mySupertype;
}

bool SchemaElement::isAbstract() const
{
  return myAbstract;
}

}
}


