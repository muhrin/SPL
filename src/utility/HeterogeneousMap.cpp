/*
 * HeterogeneousMap.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "spl/utility/HeterogeneousMap.h"

#include <utility>

#include <boost/foreach.hpp>

// NAMESPACES ////////////////////////////////

namespace spl {
namespace utility {

HeterogeneousMap::HeterogeneousMap(const HeterogeneousMap & toCopy)
{
  // Use equals operator to reduce code replication
  *this = toCopy;
}

HeterogeneousMap &
HeterogeneousMap::operator =(const HeterogeneousMap & rhs)
{
  clear();
  insert(rhs);
  return *this;
}

HeterogeneousMap::~HeterogeneousMap()
{
  clear();
}

bool
HeterogeneousMap::empty() const
{
  return myAnyMap.empty();
}

size_t
HeterogeneousMap::size() const
{
  return myAnyMap.size();
}

size_t
HeterogeneousMap::max_size() const
{
  return myAnyMap.max_size();
}

void
HeterogeneousMap::insert(const HeterogeneousMap & map)
{
  BOOST_FOREACH(const AnyMap::value_type & value, map.myAnyMap)
  {
    insert(value);
  }
}

void
HeterogeneousMap::insert(const HeterogeneousMap & map, const bool overwrite)
{
  BOOST_FOREACH(const AnyMap::value_type & value, map.myAnyMap)
  {
    insert(value, overwrite);
  }
}

void
HeterogeneousMap::clear()
{
  // Tell all the keys that they are being removed from the map
  BOOST_FOREACH(AnyMap::value_type & value, myAnyMap)
  {
    value.first->removedFromMap(*this);
  }

  myAnyMap.clear();
}

size_t
HeterogeneousMap::erase(KeyId & key)
{
  const AnyMap::iterator it = myAnyMap.find(&key);

  if(it == myAnyMap.end())
    return 0;

  myAnyMap.erase(it);
  key.removedFromMap(*this);
  return 1;
}

::std::pair< HeterogeneousMap::AnyMap::iterator, bool>
HeterogeneousMap::insert(const AnyMap::value_type & value)
{
  const ::std::pair< AnyMap::iterator, bool> result = myAnyMap.insert(value);

  if(result.second)
  {
    // Tell the key that it now stores a value in this map
    value.first->insertedIntoMap(*this);
  }

  return result;
}

::std::pair< HeterogeneousMap::AnyMap::iterator, bool>
HeterogeneousMap::insert(const AnyMap::value_type & value, const bool overwrite)
{
  ::std::pair< AnyMap::iterator, bool> result = insert(value);

  if(result.second)
  {
    // Tell the key that it now stores a value in this map
    value.first->insertedIntoMap(*this);
  }

  // If it wasn't inserted and we should overwrite the value then do so
  if(!result.second && overwrite)
  {
    result.first->second = value.second;
    result.second = true; // The value _was_ inserted
  }

  return result;
}

size_t
HeterogeneousMap::eraseNoNotify(KeyId & key)
{
  const AnyMap::iterator it = myAnyMap.find(&key);

  if(it == myAnyMap.end())
    return 0;

  myAnyMap.erase(it);
  return 1;
}

}
}
