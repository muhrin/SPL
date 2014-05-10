/*
 * Type.h
 *
 *
 *  Created on: Jan 6, 2012
 *      Author: Martin Uhrin
 */

#ifndef SCHEMER_TYPE_DETAIL_H
#define SCHEMER_TYPE_DETAIL_H

// INCLUDES /////////////////////////////////////////////

#include <boost/optional.hpp>

#include <yaml-cpp/yaml.h>

#include "schemer/Parse.h"

// DEFINES //////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////

namespace schemer {
namespace detail {

template< typename T>
  class Type
  {
  public:
    virtual
    ~Type()
    {
    }

    virtual bool
    valueToNode(const T & value, YAML::Node * const node) const = 0;
    virtual bool
    nodeToValue(const YAML::Node & node, T * const value,
        ParseLog * const log) const = 0;
    bool
    nodeToValue(const YAML::Node & node, T * const value) const
    {
      return nodeToValue(node, value, NULL);
    }

    virtual Type *
    clone() const = 0;
  };

template< typename T>
  Type< T> *
  new_clone(const Type< T> & element)
  {
    return element.clone();
  }

}
}

#endif /* SCHEMER_TYPE_DETAIL_H */
