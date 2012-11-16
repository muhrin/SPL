/*
 * IInputObject.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef I_INPUT_OBJECT_H
#define I_INPUT_OBJECT_H

// INCLUDES /////////////////////////////////////////////
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/iterator/iterator_facade.hpp>

// FOR ITERATORS
#include <iterator>

// DEFINES //////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////


namespace sstbx
{
namespace factory
{

class IInputObject
{
public:

  template <typename Value, typename Reference>
  class ObjectIterator;

  typedef ::boost::shared_ptr<IInputObject>       SharedPtrTyp;
  typedef ::boost::shared_ptr<const IInputObject> SharedPtrConstTyp;
  typedef ObjectIterator<SharedPtrConstTyp, SharedPtrConstTyp>       const_iterator;

  enum Type
  {
    UNDEFINED,
    LIST,
    MAP,
    SCALAR
  };

  template <typename T, typename U = T &>
  class IteratorBase
  {
  public:

    typedef T Value;
    typedef U Reference;

    virtual IteratorBase<Value, Reference> & operator++() = 0;
    virtual Reference operator*() = 0;
    virtual bool operator==(const IteratorBase & rhs) const = 0;
    virtual ::boost::shared_ptr<IteratorBase<Value, Reference> > clone() = 0;
  };

  template <typename Value, typename Reference, typename IterDelegate>
  class IteratorDelegator : public IteratorBase<Value, Reference>
  {
  public:
  
    typedef IteratorBase<Value, Reference> BaseTyp;

    IteratorDelegator(IterDelegate _delegate):
        myDelegate(_delegate) {}

    virtual IteratorDelegator<Value, Reference, IterDelegate> & operator++()
    {
      ++myDelegate;
      return *this;
    }

    virtual Reference operator*()
    {
      return *myDelegate;
    }

    virtual bool operator==(const BaseTyp & rhs) const
    {
      // Nastyness: have to use RTTI, twice!
      const MyTyp * const rhsDelegator = dynamic_cast<const MyTyp * const>(&rhs);

      if(!rhsDelegator)
        return false;

      const IterDelegate * const rhsDelegate = dynamic_cast<const IterDelegate * const>(rhsDelegator);

      if(!rhsDelegate)
        return false;

      // They are of the same kind so call their comparison operators
      return myDelegate == *rhsDelegate;
    }

    virtual ::boost::shared_ptr<BaseTyp> clone() const
    {
      return ::boost::shared_ptr<BaseTyp>(new MyTyp(myDelegate));
    }

  private:

    typedef IteratorDelegator<Value, Reference, IterDelegate> MyTyp;

    IterDelegate myDelegate;
  };

  template <typename Value, typename Reference = Value &>
  class ObjectIterator : public boost::iterator_facade<
        ObjectIterator<Value, Reference>,
        Value,
        boost::forward_traversal_tag,
        Reference
    >
  {
  public:

    typedef ObjectIterator<Value, Reference>    MyTyp;
    typedef IteratorBase<Value, Reference>      Delegate;

    ObjectIterator(const ::boost::shared_ptr<Delegate> & iter):
    myIter(iter)
    {}

    ObjectIterator(const MyTyp & toCopy):
      myIter(toCopy.myIter->clone())
    {}

 private:
    friend class boost::iterator_core_access;

    void increment() { myIter->operator ++(); }

    bool equal(const MyTyp & other) const
    {
        return myIter->operator==(*other.myIter);
    }

    Reference dereference() const { return *(*myIter); }

    ::boost::shared_ptr<Delegate> myIter;
  };

  virtual ~IInputObject() {}

  virtual Type                    getType() const = 0;
  virtual size_t                  getSize() const = 0;
  virtual const ::std::string &   getKey() const = 0;
  virtual const ::std::string &   getValue() const = 0;

  // Access mechanism for sequences
  virtual const SharedPtrConstTyp  operator [](const size_t idx) const = 0;

  // Access mechanism for maps
  virtual const SharedPtrConstTyp  operator [](const ::std::string & key) const = 0;

  virtual const_iterator begin() const = 0;
  virtual const_iterator end() const = 0;
};

}
}

#endif /* I_INPUT_OBJECT_H */
