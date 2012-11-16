/*
 * Enum.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef ENUM_H
#define ENUM_H

// INCLUDES /////////////////////////////////////////////
#include <algorithm>
#include <functional>
#include <set>

#include <boost/concept_check.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/static_assert.hpp>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx {
namespace utility {

template <class Derived>
class Enum
{
// FORWARD DECLARES ///////////////////////////////
protected:
  class Enumerator;

private:
  struct EnumPtrLess;
  struct EnumOrdinalCorresponds;

// END FORWARD DECLARE //////////////////////////////

public:

  // Template parameter must be a type derived from this class
  BOOST_CONCEPT_ASSERT((::boost::Convertible<Derived *, Enum<Derived> *>));

  class EnumeratorsList
  {
  private:
    typedef std::set<const Enumerator *, EnumPtrLess>  ListType;
  public:

    typedef typename ::boost::indirect_iterator< typename ListType::const_iterator > const_iterator;

    const_iterator begin() const
    {
      return const_iterator(myList.begin());
    }

    const_iterator end() const
    {
      return const_iterator(myList.end());
    }

    const Enumerator * operator[](const int ordinal) const
    {
      const typename ListType::const_iterator it =
        ::std::find_if(
          myList.begin(),
          myList.end(),
          EnumOrdinalCorresponds(ordinal)
      );

      return (it != myList.end()) ? *it : NULL;
    }
    
  private:

    EnumeratorsList() {}

    void insert(const Enumerator * const enumeration)
    {
      myList.insert(enumeration);
    }

    ListType    myList;

    friend class Enumerator;
  };

  static const EnumeratorsList & values()
  {
    return Enumerator::enumerators();
  }

  inline int ordinal() const
  {
    return myInstance->ordinal();
  }

  // OPERATORS ///////////////////////////////
  inline bool operator <(const Derived & rhs) const
  {
    return myInstance->ordinal() < rhs.myInstance->ordinal();
  }

  inline bool operator ==(const Derived & rhs) const
  {
    return myInstance == rhs.myInstance;
  }

  inline bool operator !=(const Derived & rhs) const
  {
    return myInstance != rhs.myInstance;
  }

  inline void operator =(const Derived & rhs)
  {
    myInstance = rhs.myInstance;
  }

protected:

  // Start Enum::Enumerator /////////////////////////
  class Enumerator
  {
  public:

    Enumerator():
      myOrdinal(++lastOrdinal)
    {
      enumerators().insert(this);
    }

    Enumerator(const int ordinal):
      myOrdinal(ordinal)
    {
      lastOrdinal = ordinal;
      enumerators().insert(this);
    }

    inline int ordinal() const { return myOrdinal; }

    /**
    /* Operator to create derived types i.e. 'instances' of this enumerated
    /* type.
    /**/
    operator Derived() const
    {
      Derived newT;
      newT.myInstance = this;
      return newT;
    }

  private:

    /**
    /* Have to use this static initialization technique as it is possible that
    /* an enumerator could be created before the enumerators list is.
    /* See http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.14 for details.
    /**/
    static EnumeratorsList & enumerators()
    {
      static EnumeratorsList enumerators;
      return enumerators;
    }

    static int             lastOrdinal;

    const int myOrdinal;

    friend class Enum<Derived>;
  };
  // END Enum::Enumerator ///////////////////////

  // Only derived types can instantiate
  Enum() {}

private:

  // Predicate for finding the corresponding instance
  struct EnumOrdinalCorresponds : public std::unary_function<const Enumerator, bool>
  {
    EnumOrdinalCorresponds(const int ordinal):
      myOrdinal(ordinal)
    {}

    bool operator()(const Enumerator * const enumeration)
    {
      return enumeration->ordinal() == myOrdinal;
    }

    private:
      const int myOrdinal;
  };

  // Comparison functor for the set of instances
  struct EnumPtrLess : public std::binary_function<const Enumerator *, const Enumerator *, bool>
  {
      bool operator()(const Enumerator * const enum1, const Enumerator * const enum2)
      {
        return enum1->ordinal() < enum2->ordinal();
      }
  };

  const Enumerator * myInstance;

};

// Start the list of enumerators at 0
template <class Derived>
int Enum<Derived>::Enumerator::lastOrdinal = 0;

}
}

#endif /* ENUM_H */
