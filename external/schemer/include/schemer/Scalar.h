/*
 * Scalar.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef SCHEMER_SCALAR_H
#define SCHEMER_SCALAR_H

// INCLUDES /////////////////////////////////////////////
#include <boost/optional.hpp>

#include "schemer/detail/Type.h"

// DEFINES //////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////

namespace schemer {

template< typename T>
  class Scalar : public detail::Type< T>
  {
  public:
    typedef T BindingType;

    virtual
    ~Scalar()
    {
    }

    virtual bool
    valueToNode(const T & value, YAML::Node * const node) const;
    virtual bool
    nodeToValue(const YAML::Node & node, T * const value,
        ParseLog * const log) const;
    virtual Scalar *
    clone() const;
  };

template< typename T>
  bool
  Scalar< T>::valueToNode(const T & value, YAML::Node * const node) const
  {
    // TODO: Find out how to check if transcoding failed
    *node = value;
    return true;
  }

template< typename T>
  bool
  Scalar< T>::nodeToValue(const YAML::Node & node, T * const value,
      ParseLog * const log) const
  {
    if(!node.IsScalar())
    {
      if(log)
        log->logError(ParseLogErrorCode::NODE_TYPE_WRONG, "Expected scalar.");
      return false;
    }

    try
    {
      *value = node.as< T>();
    }
    catch(const YAML::Exception & /*e*/)
    {
      if(log)
        log->logError(ParseLogErrorCode::TYPE_CONVERSION_FAILED,
            "Failed to convert scalar to bound type: " + node.Scalar());
      return false;
    }
    return true;
  }

template< typename T>
  Scalar< T> *
  Scalar< T>::clone() const
  {
    return new Scalar(*this);
  }
}

#endif /* SCHEMER_SCALAR_H */
