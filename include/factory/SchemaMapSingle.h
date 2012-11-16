/*
 * SchemaMapSingle.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef SCHEMA_SCALAR_H
#define SCHEMA_SCALAR_H

// INCLUDES /////////////////////////////////////////////
#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include "factory/ISchemaElementInstance.h"
#include "factory/SchemaElement.h"
#include "factory/SchemaScalarValue.h"
#include "factory/SchemaObjectUid.h"

// DEFINES //////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////


namespace sstbx
{
namespace factory
{

class IInputObject;

class SchemaMapSingle : public SchemaElement
{
public:

  typedef ::boost::shared_ptr<SchemaMapSingle>           Pointer;
  typedef ::boost::shared_ptr<const SchemaMapSingle>     ConstPointer;
  typedef ::boost::shared_ptr<ISchemaElementInstance> ChildPtr;
  typedef ::boost::shared_ptr<SchemaScalarValue>      ChildValuePtr;
  typedef SchemaElement::DefaultValuePointer          DefaultValuePointer;
  typedef SchemaElement::DefaultValueConstPointer     DefaultValueConstPointer;

  class Instance : public ISchemaElementInstance
  {
  public:

    void                                         setRequired(const bool required);
    void                                         setDefault(const DefaultValueConstPointer & defaultVal);

    // From ISchemaElementInstance
    virtual const SchemaMapSingle &                 getSchemaElement() const;
    virtual bool                                 isRequired() const;
    virtual int                                  getSize() const;
    virtual bool                                 validate(const IInputObject & obj, const SchemaDoc & doc) const;
    //virtual ::boost::shared_ptr<ISchemaElementInstance>    clone() const;
    virtual size_t                               numDefaults() const;
    virtual const DefaultValueConstPointer       getDefault(const size_t idx) const;
    // End from ISchemaElementInstance
  private:

    Instance(
      const SchemaMapSingle &  element,
      const bool            required,
      const IInputObject::SharedPtrConstTyp & _default);
    Instance(const Instance & toCopy);

    const SchemaMapSingle &              myElement;
    bool                              myRequired;
    DefaultValueConstPointer          myDefault;

    friend class SchemaMapSingle;
  };

  typedef Instance                    InstanceTyp;

  SchemaMapSingle(
    const ::std::string &     name,
    const bool                required     = true,
    const SchemaObjectUid &   uid          = SchemaObjectUid(),
    const IInputObject::SharedPtrConstTyp & _default = IInputObject::SharedPtrConstTyp(),
    const SchemaObjectUid     supertype    = SchemaElement::NULL_SUPERTYPE
  );

  // From SchemaElement /////////////////
  virtual size_t          numChildren() const;
  virtual const ChildPtr  operator [](const size_t idx) const;
  virtual ChildPtr        newInstance() const;
  // End from SchemaElement //////////////

  void insert(const ChildValuePtr & scalarValue);
  void insert(const ::std::string & name);

protected:
  typedef ::std::vector<ChildValuePtr> ChildrenContainer;

  const ChildrenContainer & getChildren() const;

  ChildrenContainer   myChildren;

private:

  bool                             isRequired() const;
  const DefaultValueConstPointer   getDefault() const;

  const bool                       myRequired;
  const DefaultValueConstPointer   myDefault;
  bool                             myFirstChildInserted;

  friend class Instance;
};

}
}

#endif /* SCHEMA_SCALAR_H */
