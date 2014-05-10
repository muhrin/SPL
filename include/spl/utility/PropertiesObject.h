/*
 * PropertiesObject.h
 *
 *  Created on: Sep 29, 2013
 *      Author: Martin Uhrin
 */

#ifndef PROPERTIES_OBJECT_H
#define PROPERTIES_OBJECT_H

// INCLUDES ///////////////////////////////////////////////
#include "spl/SSLib.h"

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include "spl/utility/HeterogeneousMap.h"

namespace spl {
namespace utility {

class PropertiesObject
{
public:
  PropertiesObject()
  {
  }
  PropertiesObject(const PropertiesObject & toCopy)
  {
    *this = toCopy;
  }

  PropertiesObject &
  operator =(const PropertiesObject & rhs)
  {
    myTypedProperties = rhs.myTypedProperties;
    return *this;
  }

  template< typename T>
    T *
    getProperty(const utility::Key< T> & key);

  template< typename T>
    const T *
    getProperty(const utility::Key< T> & key) const;

  template< typename T>
    void
    setProperty(utility::Key< T> & key, const T & value);

  template< typename T>
    void
    setProperty(utility::Key< T> & key, const boost::optional< T> & value);

  template< typename T>
    void
    setPropertyFromString(utility::Key< T> & key, const std::string & value);

  template< typename T>
    bool
    eraseProperty(utility::Key< T> & key);

protected:
  HeterogeneousMap &
  getProperties()
  {
    return myTypedProperties;
  }

  const HeterogeneousMap &
  getProperties() const
  {
    return myTypedProperties;
  }

private:
  HeterogeneousMap myTypedProperties;
};

template< typename T>
  T *
  PropertiesObject::getProperty(const utility::Key< T> & key)
  {
    return myTypedProperties.find(key);
  }

template< typename T>
  const T *
  PropertiesObject::getProperty(const utility::Key< T> & key) const
  {
    return myTypedProperties.find(key);
  }

template< typename T>
  void
  PropertiesObject::setProperty(utility::Key< T> & key, const T & value)
  {
    myTypedProperties[key] = value;
  }

template< typename T>
  void
  PropertiesObject::setProperty(utility::Key< T> & key,
      const boost::optional< T> & value)
  {
    if(value)
      setProperty(key, *value);
    else
      myTypedProperties.erase(key);
  }

template< typename T>
  void
  PropertiesObject::setPropertyFromString(utility::Key< T> & key,
      const std::string & value)
  {
    setProperty(key, boost::lexical_cast< T>(value));
  }

template< typename T>
  bool
  PropertiesObject::eraseProperty(utility::Key< T> & key)
  {
    return myTypedProperties.erase(key) != 0;
  }

}
}

#endif /* PROPERTIES_OBJECT_H */
