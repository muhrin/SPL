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
#include "spl/SSLib.h"

#include <ostream>
#include <set>

#include <boost/concept_check.hpp>
#include <boost/concept/assert.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#ifdef SPL_ENABLE_THREAD_AWARE
#  include <boost/thread/mutex.hpp>
#endif

namespace spl {
namespace utility {

// FORWARD DECLARATIONS ////////////////////////////////////
class HeterogeneousMap;

template <class IdType>
class HeterogeneousMapEx;

class KeyId : private ::boost::noncopyable
{
public:
  ~KeyId();

  bool operator ==(const KeyId & rhs) const
  { return this == &rhs; }

private:
  typedef ::std::set<HeterogeneousMap *> MapsSet;

  void insertedIntoMap(HeterogeneousMap & map);
  void removedFromMap(HeterogeneousMap & map);

  MapsSet myMaps;

#ifdef SPL_ENABLE_THREAD_AWARE
  ::boost::mutex myMutex;
#endif

  friend class HeterogeneousMap;
};

template <typename T>
class Key
{
public:
  typedef T ValueType;
  typedef ::boost::shared_ptr<KeyId> Id;

  Key():
  myId(new KeyId())
  {}

  Key(const Key & toCopy):
  myId(toCopy.myId)
  {}

  bool operator ==(const Key<T> & rhs) const
  { return myId.get() = rhs.myId.get(); }

  KeyId * getId()
  { return myId.get(); }

  const KeyId * getId() const
  { return myId.get(); }

private:  
  Id myId;
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

#ifdef SPL_ENABLE_THREAD_AWARE
  ::boost::mutex myMutex;
#endif

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
  virtual ~Streamable() {}
  virtual void stream(const HeterogeneousMapEx<Data> & map, ::std::ostream & stream) const = 0;
};

template <typename T, typename Data>
class StreamableKeyEx : public KeyEx<T, Data>, public Streamable<Data>
{
public:
  virtual ~StreamableKeyEx() {}
  virtual void stream(const HeterogeneousMapEx<Data> & map, ::std::ostream & stream) const;
};


}
}

#include "spl/utility/detail/HeterogeneousMapKey.h"

#endif /* HETEROGENEOUS_MAP_KEY_H */
