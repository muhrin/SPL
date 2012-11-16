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


#include <set>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>


namespace sstbx {
namespace utility {

// FORWARD DECLARATIONS ////////////////////////////////////
class HeterogeneousMap;

struct KeyId : private ::boost::noncopyable
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

template <typename Type>
struct Key
{
public:
  typedef Type ValueType;

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


}
}

#endif /* HETEROGENEOUS_MAP_KEY_H */
