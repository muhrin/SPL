/*
 * HelperTypes.h
 *
 *
 *  Created on: May 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef HELPER_TYPES_H
#define HELPER_TYPES_H

// INCLUDES /////////////////////////////////////////////
#include "spl/SSLib.h"

#ifdef SPL_WITH_YAML

// DEFINES //////////////////////////////////////////////

namespace spl {
namespace yaml {

///////////////////////////////////////////////////////////
// CLASSES
///////////////////////////////////////////////////////////

template< typename T>
  struct TypeWrapper
  {
    typedef T ValueType;

    TypeWrapper()
    {
    }
    TypeWrapper(const T & value_) :
        value(value_)
    {
    }

    T &
    operator *()
    {
      return value;
    }
    const T &
    operator *() const
    {
      return value;
    }
    T *
    operator ->()
    {
      return &value;
    }
    const T *
    operator ->() const
    {
      return &value;
    }

    TypeWrapper &
    operator =(const T & rhs)
    {
      value = rhs;
      return *this;
    }

    operator T() const
    {
      return value;
    }

    T value;
  };

// An upper triangular arma matrix so we don't need
// to store all the repeated values
typedef TypeWrapper< arma::mat> ArmaTriangularMat;

// Wrap up a vector so that it can be read as a string
// rather than as a YAML sequence
template< typename T>
  struct VecAsString
  {
    typedef TypeWrapper< std::vector< T> > Type;
  };

namespace detail {
template< typename T>
  struct ConvertStreamableScalar
  {
    static YAML::Node
    encode(const T & x)
    {
      YAML::Node node;

      std::stringstream ss;
      ss << x;
      node = ss.str();

      return node;
    }
    static bool
    decode(const YAML::Node & node, T & x)
    {
      if(!node.IsScalar())
        return false;

      std::stringstream ss(node.Scalar());
      ss >> x;

      return true;
    }
  };
}

}
}

#endif /* HELPER_TYPES_H */
#endif /* SPL_WITH_YAML */
