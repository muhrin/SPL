/*
 * HeterogeneousMapKey.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef HETEROGENEOUS_MAP_KEY_DETAIL_H
#define HETEROGENEOUS_MAP_KEY_DETAIL_H

// INCLUDES /////////////////////////////////////////////

#include <boost/foreach.hpp>
#ifdef SPL_ENABLE_THREAD_AWARE
#  include <boost/thread/locks.hpp> 
#endif

#include "spl/SSLibAssert.h"

namespace spl {
namespace utility {

template <class Data>
KeyIdEx<Data>::~KeyIdEx()
{
  // Remove all entries of ourselves from all the maps
  HeterogeneousMapEx<Data> * map;
  BOOST_FOREACH(map, myMaps)
  {
    map->erase(*this);
  }
}

template <class Data>
void KeyIdEx<Data>::insertedIntoMap(HeterogeneousMapEx<Data> & map)
{
#ifdef SPL_ENABLE_THREAD_AWARE
  ::boost::lock_guard< ::boost::mutex> guard(myMutex);
#endif
  myMaps.insert(&map);
}

template <class Data>
void KeyIdEx<Data>::removedFromMap(HeterogeneousMapEx<Data> & map)
{
#ifdef SPL_ENABLE_THREAD_AWARE
  ::boost::lock_guard< ::boost::mutex> guard(myMutex);
#endif

  typename MapsSet::iterator it = myMaps.find(&map);
  
  // We should know that we are in the map at the moment,
  // otherwise there is a problem
  SSLIB_ASSERT(it != myMaps.end());

  myMaps.erase(it);
}

template <typename T, typename Data>
void StreamableKeyEx<T, Data>::stream(const HeterogeneousMapEx<Data> & map, ::std::ostream & stream) const
{
  const T * const value = map.find(*this);
  if(value)
    stream << *value;
}

}
}

#endif /* HETEROGENEOUS_MAP_KEY_DETAIL_H */
