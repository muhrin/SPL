/*
 * SchemaObjectUid.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef SCHEMA_OBJECT_UID_H
#define SCHEMA_OBJECT_UID_H

// INCLUDES /////////////////////////////////////////////
#include <string>

// DEFINES //////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////


namespace sstbx
{
namespace factory
{

class SchemaObjectUid
{
public:
  SchemaObjectUid();
  SchemaObjectUid(const ::std::string & uidStr);
  SchemaObjectUid(const SchemaObjectUid & uid);
  SchemaObjectUid(const char * const uidChar);
  const ::std::string & get() const;
  SchemaObjectUid & operator =(const SchemaObjectUid & uid);
  bool operator ==(const SchemaObjectUid & rhs) const;
  bool operator <(const SchemaObjectUid & rhs) const;
private:
  ::std::string   myUidStr;
};

}
}

#endif /* SCHEMA_OBJECT_UID_H */
