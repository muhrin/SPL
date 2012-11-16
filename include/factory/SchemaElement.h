/*
 * SchemaElement.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef SCHEMA_TYPE_H
#define SCHEMA_TYPE_H

// INCLUDES /////////////////////////////////////////////

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "factory/IInputObject.h"
#include "factory/SchemaObjectUid.h"

// DEFINES //////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////


namespace sstbx
{
namespace factory
{

class ISchemaElementInstance;
class ITypeInstance;
class SchemaDoc;

class SchemaElement
{
public:

  typedef ::boost::shared_ptr<SchemaElement>          SharedPtrTyp;
  typedef ::boost::shared_ptr<ISchemaElementInstance> ChildPtr;
  typedef ::boost::shared_ptr<IInputObject>           DefaultValuePointer;
  typedef ::boost::shared_ptr<const IInputObject>     DefaultValueConstPointer;

  static const SchemaObjectUid    NULL_SUPERTYPE;
  static const SchemaObjectUid    UID_ANONYMOUS;

  virtual ~SchemaElement() {}

  const SchemaObjectUid &              getUid() const;
  const ::std::string &                getName() const;
  IInputObject::Type                   getType() const;
  const SchemaObjectUid &              getSupertype() const;
  bool                                 isAbstract() const;

  // Virtual methods //////////////////
  virtual size_t                    numChildren() const = 0;
  virtual const ChildPtr            operator[](const size_t idx) const = 0;
  virtual ChildPtr                  newInstance() const = 0;
  // End virtual methods /////////////

protected:

  SchemaElement(
    const IInputObject::Type        type,
    const ::std::string &           name,
    const SchemaObjectUid &         uid        = SchemaObjectUid(),
    const bool                      isAbstract = false,
    const SchemaObjectUid           supertype  = NULL_SUPERTYPE
  );

private:

  const SchemaObjectUid          myUid;
  const IInputObject::Type       myType;
  const ::std::string            myName;
  const SchemaObjectUid          mySupertype;
  const bool                     myAbstract;
};

}
}

#endif /* SCHEMA_TYPE_H */
