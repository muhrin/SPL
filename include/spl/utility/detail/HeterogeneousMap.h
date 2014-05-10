/*
 * HeterogeneousMap.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef HETEROGENEOUS_MAP_DETAIL_H
#define HETEROGENEOUS_MAP_DETAIL_H

// INCLUDES /////////////////////////////////////////////

// FORWARD DECLARATIONS ////////////////////////////////////

namespace spl {
namespace utility {

template< typename T>
  bool
  HeterogeneousMap::insert(Key< T> & key, T value)
  {
    return insert(AnyMap::value_type(key.getId(), value)).second;
  }

template< typename T>
  bool
  HeterogeneousMap::insert(value_type< T> & x)
  {
    return insert(AnyMap::value_type(x.first.getId(), x.second)).second;
  }

template< typename T>
  T &
  HeterogeneousMap::operator [](Key< T> & key)
  {
    T * value = find(key);

    if(!value)
    {
      // Insert default value
      ::std::pair< AnyMap::iterator, bool> result = insert(
          AnyMap::value_type(key.getId(), T()));
      value = ::boost::any_cast< T>(&result.first->second);
    }

    return *value;
  }

template< typename T>
  T *
  HeterogeneousMap::find(const Key< T> & key)
  {
    // NOTE: Have to use const_cast<> here as we have a type of const KeyId * but the
    // map key is actually of type KeyId *.  In this case the cast is safe to do as we
    // guarantee that we will not modify the key.
    const AnyMap::iterator it = myAnyMap.find(
        const_cast< KeyId *>(key.getId()));

    if(it == myAnyMap.end())
      return NULL;

    return ::boost::any_cast< T>(&it->second);
  }

template< typename T>
  const T *
  HeterogeneousMap::find(const Key< T> & key) const
  {
    // NOTE: Have to use const_cast<> here as we have a type of const KeyId * but the
    // map key is actually of type KeyId *.  In this case the cast is safe to do as we
    // guarantee that we will not modify the key.
    const AnyMap::const_iterator it = myAnyMap.find(
        const_cast< KeyId *>(key.getId()));

    if(it == myAnyMap.end())
      return NULL;

    return ::boost::any_cast< T>(&it->second);
  }

template< typename T>
  size_t
  HeterogeneousMap::erase(Key< T> & key)
  {
    const AnyMap::iterator it = myAnyMap.find(key.getId());

    if(it == myAnyMap.end())
      return 0;

    myAnyMap.erase(it);
    key.getId()->removedFromMap(*this);
    return 1;
  }

}
}

#endif /* HETEROGENEOUS_MAP_DETAIL_H */
