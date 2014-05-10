/*
 * Enumeration.h
 *
 *
 *  Created on: Jan 3, 2013
 *      Author: Martin Uhrin
 */

#ifndef SCHEMER_ENUMERATION_DETAIL_H
#define SCHEMER_ENUMERATION_DETAIL_H

// INCLUDES /////////////////////////////////////////////

// DEFINES //////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////

namespace schemer {

template< typename T>
  void
  Enumeration< T>::enumeration(const ::std::string & name, const T value)
  {
    myEnumMap[name] = value;
  }

template< typename T>
  bool
  Enumeration< T>::valueToNode(const T & value, YAML::Node * const node) const
  {
    typename ::std::map< ::std::string, T>::const_iterator it =
        myEnumMap.begin();
    for(const typename ::std::map< ::std::string, T>::const_iterator end =
        myEnumMap.end(); it != end; ++it)
    {
      if(it->second == value)
        break;
    }

    if(it == myEnumMap.end())
      return false;

    *node = it->first;
    return true;
  }

template< typename T>
  bool
  Enumeration< T>::nodeToValue(const YAML::Node & node, T * const value,
      ParseLog * const log) const
  {
    typename ::std::map< ::std::string, T>::const_iterator it = myEnumMap.find(
        node.Scalar());

    if(it == myEnumMap.end())
    {
      // TODO: Parse error: unrecognised enum value
      return false;
    }
    *value = it->second;
    return true;
  }

template< typename T>
  Enumeration< T> *
  Enumeration< T>::clone() const
  {
    return new Enumeration(*this);
  }

}

#endif /* SCHEMER_ENUMERATION_DETAIL_H */
