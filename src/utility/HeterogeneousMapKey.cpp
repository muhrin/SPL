/*
 * HeterogenousMapKey.cpp
 *
 *  Created on: Sep 3, 2012
 *      Author: Martin Uhrin
 */

// INCLUDES /////////////////////////////////////
#include "utility/HeterogeneousMapKey.h"

#include <boost/foreach.hpp>

#include "SSLibAssert.h"
#include "utility/HeterogeneousMap.h"

namespace sstbx {
namespace utility {

KeyId::~KeyId()
{
  // Remove all entries of ourselves from all the maps
  HeterogeneousMap * map;
  BOOST_FOREACH(map, myMaps)
  {
    map->erase(*this);
  }
}

void KeyId::insertedIntoMap(HeterogeneousMap & map)
{
  myMaps.insert(&map);
}

void KeyId::removedFromMap(HeterogeneousMap & map)
{
  MapsSet::iterator it = myMaps.find(&map);
  
  // We should know that we are in the map at the moment,
  // otherwise there is a problem
  SSLIB_ASSERT(it != myMaps.end());

  myMaps.erase(it);
}

}
}
