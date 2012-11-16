/*
 * SchemaObjectUid.cpp
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "factory/SchemaObjectUid.h"

// NAMESPACES ////////////////////////////////

namespace sstbx
{
namespace factory
{

SchemaObjectUid::SchemaObjectUid()
{}

SchemaObjectUid::SchemaObjectUid(const ::std::string & uidStr):
myUidStr(uidStr)
{}

SchemaObjectUid::SchemaObjectUid(const char * const uidChar):
myUidStr(uidChar)
{}

SchemaObjectUid::SchemaObjectUid(const SchemaObjectUid & uid):
myUidStr(uid.get())
{}

const ::std::string & SchemaObjectUid::get() const
{
  return myUidStr;
}

SchemaObjectUid & SchemaObjectUid::operator =(const SchemaObjectUid & uid)
{
  myUidStr = uid.get();
  return *this;
}

bool SchemaObjectUid::operator ==(const SchemaObjectUid & rhs) const
{
  return myUidStr == rhs.get();
}


bool SchemaObjectUid::operator <(const SchemaObjectUid & rhs) const
{
  return myUidStr < rhs.get();
}

}
}


