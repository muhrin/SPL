/*
 * SchemaList.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef SCHEMA_LIST_H
#define SCHEMA_LIST_H

// INCLUDES /////////////////////////////////////////////
#include <string>
#include <vector>

#include "factory/ISchemaElementInstance.h"
#include "factory/SchemaElement.h"
#include "factory/SchemaObjectUid.h"

// DEFINES //////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx
{
namespace factory
{

class SchemaDoc;

class SchemaList : public SchemaElement
{
protected:
    
public:

  typedef SchemaElement::ChildPtr                     ChildPtr;
  typedef SchemaElement::DefaultValuePointer          DefaultValuePointer;
  typedef SchemaElement::DefaultValueConstPointer     DefaultValueConstPointer;

  class Instance : public ISchemaElementInstance
  {
  public:

    // From ISchemaElementInstance ////////////////////
    virtual const SchemaList &                    getSchemaElement() const;
    virtual bool                                  isRequired() const;
    virtual int                                   getSize() const;
    virtual bool                                  validate(const IInputObject & obj, const SchemaDoc & doc) const;
    virtual ISchemaElementInstance::SharedPtrTyp  clone() const;
    virtual size_t                                numDefaults() const;
    virtual const DefaultValueConstPointer        getDefault(const size_t idx) const;
    // End from ISchemaElementInstance /////////////////

  private:

    typedef ::std::vector<DefaultValueConstPointer> DefaultsContainer;

    Instance(const SchemaList & element, const bool required, const int size);
    Instance(const Instance & toCopy);

    const SchemaList &    myElement;
    bool                  myRequired;
    int                   mySize;
    DefaultsContainer     myDefaults;

    friend class SchemaList;
  };

  typedef Instance                                    InstanceTyp;

  SchemaList(
    const ::std::string &     name,
    const bool                required     = true,
    const SchemaObjectUid &   uid          = SchemaObjectUid(),
    const int                 size         = ISchemaElementInstance::SIZE_UNDEFINED
  );

  // From SchemaElement ////////////////////
  virtual size_t            numChildren() const;
  virtual const ChildPtr    operator[](const size_t idx) const;
  virtual ChildPtr          newInstance() const;
  // End from SchemaElement ////////////////

  void insert(const ChildPtr & elementInst);

protected:

  typedef ::std::vector<ChildPtr>       ChildrenContainer;

  int               getSize() const;
  bool              isRequired() const;

  const int         mySize;
  const bool        myRequired;
  ChildrenContainer myChildren;

  friend class Instance;
};

}
}

#endif /* SCHEMA_LIST_H */
