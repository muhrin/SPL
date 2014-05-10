/*
 * NamedProperty.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef NAMED_PROPERTY_DETAIL_H
#define NAMED_PROPERTY_DETAIL_H

// INCLUDES /////////////////////////////////////////////

#include <string>
#include <ostream>

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include "spl/utility/HeterogeneousMap.h"

// FORWARD DECLARATIONS ////////////////////////////////////


namespace spl {
namespace utility {

class BadLexicalCast : public ::std::exception
{
public:
  BadLexicalCast() {}
  BadLexicalCast(const char * msg);

  virtual ~BadLexicalCast() throw() {}

  virtual const char * what() const throw();

private:
  ::std::string myMsg;
};

template <typename T>
struct Convert
{
  static ::std::string toString(const T & value)
  {
    return ::boost::lexical_cast< ::std::string>(value);
  }

  static T fromString(const ::std::string & stringValue)
  {
    try
    {
      return ::boost::lexical_cast<T>(stringValue);
    }
    catch(const ::boost::bad_lexical_cast & /*e*/)
    {
      throw BadLexicalCast();
    }
  }
private:
  Convert() {}
};

template <>
struct Convert< ::std::string>
{
  static ::std::string toString(const ::std::string & value)
  {
    return value;
  }

  static ::std::string fromString(const ::std::string & stringValue)
  {
    return stringValue;
  }
};

template <typename T>
NamedKey<T>::NamedKey(const std::string & name):
myName(name)
{}

template <typename T>
::std::string NamedKey<T>::getName() const
{
  return myName;
}

template <typename T>
::boost::optional< ::std::string>
NamedKey<T>::getValue(const spl::utility::HeterogeneousMap & container) const
{
  ::boost::optional< ::std::string> stringValue;

  const T * const value = container.find(*this);
  if(value)
  {
    stringValue.reset(Convert<T>::toString(*value));
  }

  return stringValue;
}

template <typename T>
bool
NamedKey<T>::setValue(HeterogeneousMap & container, const ::std::string & valueString)
{
  try
  {
    container[*this] = Convert<T>::fromString(valueString);
  }
  catch(const BadLexicalCast & /*exception*/)
  {
    return false;
  }

  return true;
}


template <typename Container>
bool NamedPropertyStore<Container>::addProperty(NamedProperty<Container> & namedProperty)
{
  return myProperties.insert(std::make_pair(namedProperty.getName(), &namedProperty)).second;
}

template <typename Container>
bool NamedPropertyStore<Container>::removeProperty(NamedProperty<Container> & namedProperty)
{
  typename Properties::iterator it = myProperties.find(namedProperty.getName());

  if(it == myProperties.end())
    return false;

  myProperties.erase(it);
  return true;
}

template <typename Container>
NamedProperty<Container> * NamedPropertyStore<Container>::getProperty(const std::string & name)
{
  typename Properties::iterator it = myProperties.find(name);

  if(it == myProperties.end())
    return NULL;

  return it->second;
}

template <typename Container>
typename NamedPropertyStore<Container>::iterator
NamedPropertyStore<Container>::begin()
{
  return iterator(myProperties.begin());
}

template <typename Container>
typename NamedPropertyStore<Container>::const_iterator
NamedPropertyStore<Container>::begin() const
{
  return iterator(myProperties.begin());
}


template <typename Container>
typename NamedPropertyStore<Container>::iterator
NamedPropertyStore<Container>::end()
{
  return iterator(myProperties.end());
}

template <typename Container>
typename NamedPropertyStore<Container>::const_iterator
NamedPropertyStore<Container>::end() const
{
  return iterator(myProperties.end());
}

}
}

#endif /* NAMED_PROPERTY_DETAIL_H */
