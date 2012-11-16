/*
 * ISchemaElementInstance.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef I_TYPE_INSTANCE_H
#define I_TYPE_INSTANCE_H

// INCLUDES /////////////////////////////////////////////

#include <boost/shared_ptr.hpp>

// DEFINES //////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////


namespace sstbx
{
namespace factory
{

class IInputObject;
class SchemaDoc;
class SchemaElement;

class ISchemaElementInstance
{
public:

  typedef ::boost::shared_ptr<ISchemaElementInstance> SharedPtrTyp;
  typedef ::boost::shared_ptr<const IInputObject>     DefaultValueConstPtr;

  static const int SIZE_UNDEFINED = -1;

  virtual ~ISchemaElementInstance() {}

  virtual const SchemaElement &              getSchemaElement() const = 0;
  virtual bool                               isRequired() const = 0;
  virtual int                                getSize() const = 0;
  virtual bool                               validate(const IInputObject & obj, const SchemaDoc & doc) const = 0;
  //virtual SharedPtrTyp                       clone() const = 0;
  // Default entries
  virtual size_t                             numDefaults() const = 0;
  virtual const DefaultValueConstPtr         getDefault(const size_t idx) const = 0;

};

}
}

#endif /* I_TYPE_INSTANCE_H */
