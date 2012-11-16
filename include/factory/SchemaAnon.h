/*
 * SchemaAnon.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef SCHEMA_ANON_H
#define SCHEMA_ANON_H

// INCLUDES /////////////////////////////////////////////
#include <boost/concept_check.hpp>
#include <boost/concept/assert.hpp>
#include <boost/shared_ptr.hpp>


#include "factory/ISchemaElementInstance.h"
#include "factory/SchemaElement.h"

// DEFINES //////////////////////////////////////////////

namespace sstbx
{
namespace factory
{

// FORWARD DECLARATIONS ////////////////////////////////////

template <class T>
class SchemaAnon : public ISchemaElementInstance
{
public:

  typedef T                                     ElementTyp;
  typedef typename ElementTyp::InstanceTyp      InstanceTyp;
  typedef ::boost::shared_ptr<ElementTyp>       ElementPtr;
  typedef ::boost::shared_ptr<InstanceTyp>      InstancePtr;
  typedef typename ISchemaElementInstance::DefaultValueConstPtr
    DefaultValueConstPtr;

  // Template concept checks
  BOOST_CONCEPT_ASSERT((::boost::Convertible<ElementTyp*, SchemaElement*>));

  static ::boost::shared_ptr<SchemaAnon<T> > create(const ElementPtr & element);

  SchemaAnon(const ElementPtr element);

  // From ISchemaElementInstance ///////////////////////
  virtual const SchemaElement &              getSchemaElement() const;
  virtual bool                               isRequired() const;
  virtual int                                getSize() const;
  virtual bool                               validate(const IInputObject & obj, const SchemaDoc & doc) const;
  //virtual ISchemaElementInstance::SharedPtrTyp    clone() const;
  // Default entries
  virtual size_t                             numDefaults() const;
  virtual const DefaultValueConstPtr         getDefault(const size_t idx) const;
  // From ISchemaElementInstance ///////////////////////

private:

  const ElementPtr  myElement;
  //InstancePtr       myInstance;
  ::boost::shared_ptr<ISchemaElementInstance> myInstance;
  

};


// IMPLEMENTATION ////////////////////////////////

template <class T>
::boost::shared_ptr<SchemaAnon<T> >
SchemaAnon<T>::create(const typename SchemaAnon<T>::ElementPtr & element)
{
  return typename ::boost::shared_ptr<SchemaAnon<T> >(new SchemaAnon<T>(element));
}

template <class T>
SchemaAnon<T>::SchemaAnon(const typename SchemaAnon<T>::ElementPtr element):
myElement(element),
myInstance(myElement->newInstance())
{}

template <class T>
const SchemaElement & SchemaAnon<T>::getSchemaElement() const
{
  return myInstance->getSchemaElement();
}

template <class T>
bool SchemaAnon<T>::isRequired() const
{
  return myInstance->isRequired();
}

template <class T>
int SchemaAnon<T>::getSize() const
{
  return myInstance->getSize();
}

template <class T>
bool SchemaAnon<T>::validate(const sstbx::factory::IInputObject &obj, const sstbx::factory::SchemaDoc &doc) const
{
  return myInstance->validate(obj, doc);
}

template <class T>
size_t SchemaAnon<T>::numDefaults() const
{
  return myInstance->numDefaults();
}

template <class T>
const typename SchemaAnon<T>::DefaultValueConstPtr
SchemaAnon<T>::getDefault(const size_t idx) const
{
  return myInstance->getDefault(idx);
}

}
}

#endif /* SCHEMA_ANON_H */

