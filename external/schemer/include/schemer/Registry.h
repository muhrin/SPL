/*
 * Registry.h
 *
 *  Created on: Dec 29, 2013
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#ifndef REGISTRY_H
#define REGISTRY_H

#include <map>
#include <vector>

#include <boost/type_traits/is_fundamental.hpp>

#include "schemer/Enumeration.h"
#include "schemer/List.h"
#include "schemer/Map.h"
#include "schemer/Scalar.h"

#define SCHEMER_REGISTER(SCHEMA_TYPE, BINDING_TYPE) \
  const SCHEMA_TYPE & getType(const BINDING_TYPE &) \
  { return ::schemer::getTypeInstance< SCHEMA_TYPE>(); }

#define SCHEMER_MAP(NAME, TYPE) \
  struct NAME : public ::schemer::HeteroMap< TYPE> \
  { NAME(); }; \
  inline const NAME & getType(const TYPE &) \
  { return ::schemer::getTypeInstance< NAME>(); } \
  inline NAME::NAME()

#define SCHEMER_HOMO_MAP(NAME) SCHEMER_HOMO_MAP_TYPED(NAME, ::schemer::String)

#define SCHEMER_HOMO_MAP_TYPED(NAME, TYPE) \
  SCHEMER_HOMO_MAP_KEY_TYPED(NAME, ::schemer::String, TYPE)

#define SCHEMER_HOMO_MAP_KEY_TYPED(NAME, KEY, TYPE) \
  struct NAME : public ::schemer::Map< KEY, TYPE> \
  { NAME(); }; \
  inline NAME::NAME()

#define SCHEMER_ENUM(NAME, TYPE) \
  struct NAME : public ::schemer::Enumeration< TYPE> \
  { NAME(); }; \
  inline const NAME & getType(const TYPE & ) \
  { return ::schemer::getTypeInstance< NAME>(); } \
  inline NAME::NAME()

#define SCHEMER_LIST(NAME, TYPE) \
  struct NAME : public ::schemer::List< TYPE> \
  { NAME(); }; \
  inline NAME::NAME()

namespace schemer {

template< typename T>
  const T &
  getTypeInstance()
  {
    static const T TYPE;
    return TYPE;
  }

template< typename T, bool IsCppFundamental = boost::is_fundamental< T>::value>
  struct TypeGetter
  {
  };

inline const Scalar< std::string> &
getType(const std::string &)
{
  return getTypeInstance< Scalar< std::string> >();
}

// Fall-through: assume it's a scalar if nothing else is known
template< typename T>
  const Scalar< T> &
  getType(const T &)
  {
    return getTypeInstance< Scalar< T> >();
  }

template< typename EntryType>
  const List< EntryType> &
  makeList(const EntryType & entry)
  {
    return getTypeInstance< List< EntryType> >();
  }

template< typename T>
  static const detail::Type< std::vector< T> > &
  getType(const std::vector< T> &)
  {
    return makeList(getType(T()));
  }

template< typename K, typename T>
  const Map< Scalar< K>, Scalar< T> > &
  getType(const std::map< K, T> &)
  {
    return getTypeInstance< Map< Scalar< K>, Scalar< T> > >();
  }

// Use boost::is_fundamental to automatically create scalar types for fundamental
// types
template< typename T>
  struct TypeGetter< T, true>
  {
    static const Scalar< T> &
    get()
    {
      // If we know nothing else, assume it's a scalar.
      return getTypeInstance< Scalar< T> >();
    }
  };

template< typename T>
  struct TypeGetter< T, false>
  {
    static const detail::Type< T> &
    get()
    {
      // Try ADL
      return getType(T());
    }
  };

template< typename T>
  const detail::Type< T> &
  getType()
  {
    return TypeGetter< T>::get();
  }

}

#endif // REGISTRY_H
