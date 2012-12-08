/*
 * NamedProperty.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef NAMED_PROPERTY_H
#define NAMED_PROPERTY_H

// INCLUDES /////////////////////////////////////////////

#include <string>
#include <ostream>

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include "utility/HeterogeneousMap.h"

// FORWARD DECLARATIONS ////////////////////////////////////


namespace sstbx {
namespace utility {

template <typename Container>
class NamedProperty
{
public:
  virtual ~NamedProperty() {}

  virtual ::std::string getName() const = 0;
  virtual ::boost::optional< ::std::string> getValue(const Container & container) const = 0;
  //virtual bool streamValue(const Container & container, ::std::ostream & stream) = 0;
  virtual bool setValue(Container & container, const ::std::string & valueString) const = 0;
};

template <typename T>
class NamedKey : public Key<T>, public NamedProperty<HeterogeneousMap>
{
public:
  NamedKey(::std::string & name);

  // From NamedProperty //////////////////
  virtual ::std::string getName() const;
  virtual ::boost::optional< ::std::string> getValue(const HeterogeneousMap & container) const;
  //virtual bool streamValue(const HeterogeneousMap & container, ::std::ostream & stream);
  virtual bool setValue(HeterogeneousMap & container, const ::std::string & valueString) const;

private:

  const ::std::string myName;


};

// IMPLEMENTATION //////////////////
template <typename T>
NamedKey<T>::NamedKey(std::string &name):
myName(name)
{}

template <typename T>
::std::string NamedKey<T>::getName() const
{
  return myName;
}

template <typename T>
::boost::optional< ::std::string>
NamedKey<T>::getValue(const sstbx::utility::HeterogeneousMap & container) const
{
  ::boost::optional< ::std::string> stringValue;

  const T * const value = container.find(*this);
  if(value)
  {
    stringValue.reset(convert<T>::toString(*value));
  }

  return stringValue;
}

template <typename T>
bool
NamedKey<T>::setValue(HeterogeneousMap & container, const ::std::string & valueString) const
{
  try
  {
    container[*this] = convert<T>::fromString(valueString);
  }
  catch(catch BadLexicalCast & exception)
  {
    return false;
  }

  return true;
}


}
}

#endif /* NAMED_PROPERTY_H */
