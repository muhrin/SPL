/*
 * HeterogeneousMapKey.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef HETEROGENEOUS_MAP_KEY_H
#define HETEROGENEOUS_MAP_KEY_H

// INCLUDES /////////////////////////////////////////////

#include <ostream>
#include <set>

#include <boost/concept_check.hpp>
#include <boost/concept/assert.hpp>
#include <boost/foreach.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>


#include "SSLibAssert.h"

namespace sstbx {
namespace utility {

// FORWARD DECLARATIONS ////////////////////////////////////
class HeterogeneousMap;

template <class IdType>
class HeterogeneousMapEx;

class KeyId : private ::boost::noncopyable
{
public:
  ~KeyId();

private:

  typedef ::std::set<HeterogeneousMap *> MapsSet;

  void insertedIntoMap(HeterogeneousMap & map);
  void removedFromMap(HeterogeneousMap & map);

  MapsSet myMaps;

  friend class HeterogeneousMap;
};

template <typename T>
class Key
{
public:
  typedef T ValueType;

  Key():
  myId(new KeyId())
  {}

  Key(Key & toCopy):
  myId(toCopy.myId)
  {}

private:

  typedef ::boost::shared_ptr<KeyId> Id;

  KeyId * getId()
  { return myId.get(); }

  const KeyId * getId() const
  { return myId.get(); }

  Id myId;

  friend class HeterogeneousMap;
};

template <class Data>
class KeyIdEx : private ::boost::noncopyable
{
public:

  KeyIdEx(const Data & data):
  myData(data)
  {}

  ~KeyIdEx();

  const Data & getData()
  { return myData; }

private:

  typedef ::std::set<HeterogeneousMapEx<Data> *> MapsSet;

  void insertedIntoMap(HeterogeneousMapEx<Data> & map);
  void removedFromMap(HeterogeneousMapEx<Data> & map);

  MapsSet myMaps;

  const Data myData;

  friend class HeterogeneousMapEx<Data>;
};


template <typename T, typename Data>
class KeyEx
{
public:
  typedef T ValueType;

public:

  KeyEx(const Data & data):
  myId(new KeyIdEx<Data>(data))
  {}

  KeyEx(KeyEx & toCopy):
  myId(toCopy.myId)
  {}

  const Data & getData()
  { return myId->getData(); }

private:

  typedef KeyIdEx<Data> IdType;
  typedef ::boost::shared_ptr<IdType> Id;

  IdType * getId()
  { return myId.get(); }

  const IdType * getId() const
  { return myId.get(); }

  Id myId;

  friend class HeterogeneousMapEx<Data>;
};

template <typename Data>
class Streamable
{
public:
  virtual void stream(const HeterogeneousMapEx<Data> & map, ::std::ostream & stream) const = 0;
};

template <typename T, typename Data>
class StreamableKeyEx : public KeyEx<T, Data>, public Streamable<Data>
{
public:
  virtual void stream(const HeterogeneousMapEx<Data> & map, ::std::ostream & stream) const;
};


// IMPLEMENTATION /////////////////////////

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
  myMaps.insert(&map);
}

template <class Data>
void KeyIdEx<Data>::removedFromMap(HeterogeneousMapEx<Data> & map)
{
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

#endif /* HETEROGENEOUS_MAP_KEY_H */
