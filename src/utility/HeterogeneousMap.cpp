/*
 * HeterogeneousMap.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "utility/HeterogeneousMap.h"

#include <boost/foreach.hpp>

// NAMESPACES ////////////////////////////////

namespace sstbx {
namespace utility {

HeterogeneousMap::~HeterogeneousMap()
{
  clear();
}

bool HeterogeneousMap::empty() const
{
  return myAnyMap.empty();
}

size_t HeterogeneousMap::size() const
{
  return myAnyMap.size();
}

size_t HeterogeneousMap::max_size() const
{
  return myAnyMap.max_size();
}

void HeterogeneousMap::clear()
{
  // Tell all the keys that they are being removed from the map
  BOOST_FOREACH(AnyMap::value_type & value, myAnyMap)
  {
    value.first->removedFromMap(*this);
  }

  myAnyMap.clear();
}

size_t HeterogeneousMap::erase(KeyId & key)
{
  const AnyMap::iterator it = myAnyMap.find(&key);

  if(it == myAnyMap.end())
    return 0;

  myAnyMap.erase(it);
  key.removedFromMap(*this);
  return 1;
}

}
}
