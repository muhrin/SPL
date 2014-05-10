/*
 * Range.h
 *
 * Tuple that represents a range.
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef RANGE_H
#define RANGE_H

// INCLUDES /////////////////////////////////////////////
#include "spl/SSLib.h"

#include <iostream>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/concept/assert.hpp>
#include <boost/concept_check.hpp>
#include <boost/lexical_cast.hpp>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace utility {

template< typename T>
  class OrderedPair
  {
  public:
    BOOST_CONCEPT_ASSERT((boost::LessThanComparable<T>));

    typedef T ValueType;

    static OrderedPair make(const T & x0, const T & x1)
    {
      return OrderedPair(x0, x1);
    }

    OrderedPair()
    {}

    explicit OrderedPair(const T & x)
    {
      set(x, x);
    }

    OrderedPair(const T & x0, const T & x1)
    {
      set(x0, x1);
    }

    OrderedPair(const OrderedPair & toCopy)
    {
      myFirst = toCopy.myFirst;
      mySecond = toCopy.mySecond;
    }

    OrderedPair & operator =(const OrderedPair & rhs)
    {
      myFirst = rhs.myFirst;
      mySecond = rhs.mySecond;
      return *this;
    }

    bool operator ==(const OrderedPair & rhs) const
    {
      return myFirst == rhs.myFirst && mySecond == rhs.mySecond;
    }

    const T & first() const
    { return myFirst;}

    const T & second() const
    { return mySecond;}

    void setBoth(const T & x)
    {
      set(x, x);
    }

    void set(const T & x0, const T & x1)
    {
      myFirst = x0 < x1 ? x0 : x1;
      mySecond = x0 < x1 ? x1 : x0;
    }

    bool operator < (const OrderedPair & rhs) const
    {
      if(myFirst < rhs.myFirst)
      return true;
      if(rhs.myFirst < myFirst)
      return false;

      if(mySecond < rhs.mySecond)
      return true;

      return false;
    }

  protected:
    T myFirst;
    T mySecond;
  };

template< typename T>
  class Range
  {
  public:
    BOOST_CONCEPT_ASSERT((boost::LessThanComparable<T>));

    typedef T ValueType;

    static Range make(const T & x0, const T & x1)
    {
      return Range(x0, x1);
    }

    Range()
    {}

    Range(const T & x0, const T & x1):
    myPair(x0, x1)
    {
    }

    Range & operator =(const Range & rhs)
    {
      myPair = rhs.myPair;
      return *this;
    }

    bool operator ==(const Range & rhs) const
    {
      return myPair == rhs.myPair;
    }

    const T &
    min() const
    {
      return myPair.first();
    }

    const T &
    max() const
    {
      return myPair.second();
    }

    bool nullSpan() const
    { return myPair.first() == myPair.second();}

    T span() const
    { return myPair.second() - myPair.first();}

    void
    setBoth(const T & x)
    {
      myPair.setBoth(x);
    }

    void
    set(const T & x0, const T & x1)
    {
      myPair.set(x0, x1);
    }

    bool
    expand(const T & x)
    {
      if(x < myPair.first())
      {
        myPair.set(x, myPair.upper());
        return true;
      }

      if(x > myPair.upper())
      {
        myPair.set(myPair.first(), x);
        return true;
      }
      return false;
    }

  private:
    OrderedPair<T> myPair;
  };

// IMPLEMENTATION /////////////////////////////////////////////
template< typename T>
  std::ostream &
  operator <<(std::ostream & out, const OrderedPair< T> & pair)
  {
    out << pair.first() << "~" << pair.second();
    return out;
  }

template< typename T>
  std::istream &
  operator >>(std::istream &in, OrderedPair< T> & pair)
  {
    using boost::lexical_cast;

    std::string rangeString;
    while(in.good() && rangeString.empty())
      in >> rangeString;

    const size_t delimPos = rangeString.find('~');
    if(delimPos == std::string::npos)
      pair.setBoth(lexical_cast< T>(rangeString));
    else
      pair.set(lexical_cast< T>(rangeString.substr(0, delimPos)),
          lexical_cast< T>(
              rangeString.substr(delimPos + 1, rangeString.size())));

    return in;
  }

template< typename T>
  std::ostream &
  operator <<(std::ostream & out, const Range< T> & range)
  {
    if(range.nullSpan())
      out << range.min();
    else
      out << range.min() << "~" << range.max();
    return out;
  }

template< typename T>
  std::istream &
  operator >>(std::istream &in, Range< T> & range)
  {
    using boost::lexical_cast;

    std::string rangeString;
    while(in.good() && rangeString.empty())
      in >> rangeString;

    const size_t delimPos = rangeString.find('~');
    if(delimPos == std::string::npos)
      range.setBoth(lexical_cast< T>(rangeString));
    else
      range.set(lexical_cast< T>(rangeString.substr(0, delimPos)),
          lexical_cast< T>(
              rangeString.substr(delimPos + 1, rangeString.size())));

    return in;
  }
}
}

#endif /* RANGE_H */
