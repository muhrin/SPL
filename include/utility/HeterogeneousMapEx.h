/*
 * HeterogeneousMapEx.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef HETEROGENEOUS_MAP_EX_H
#define HETEROGENEOUS_MAP_EX_H

// INCLUDES /////////////////////////////////////////////

#include <map>

#include <boost/any.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/utility.hpp>

#include <utility/HeterogeneousMapKey.h>

namespace sstbx {
namespace utility {

// FORWARD DECLARATIONS ////////////////////////////////////


template <class KeyData>
class HeterogeneousMapEx
{

  typedef KeyIdEx<KeyData> IdType;
  typedef ::std::map<IdType *, ::boost::any> AnyMap;
public:

  template <typename T>
  struct value_type : public ::std::pair<KeyEx<T, KeyData>, T>
  {};

  HeterogeneousMapEx() {}
  HeterogeneousMapEx(const HeterogeneousMapEx & toCopy);
  ~HeterogeneousMapEx();
 
  // Capacity methods
  bool empty() const;
  size_t size() const;
  size_t max_size() const;

	template <typename T>
	bool insert(KeyEx<T, KeyData> & key, T value);

  template <typename T>
  bool insert(value_type<T> & x);

  template <typename T>
  T & operator [](KeyEx<T, KeyData> & x);

	template<typename T>
	T * find(const KeyEx<T, KeyData> & key);

	template<typename T>
	const T * find(const KeyEx<T, KeyData> & key) const;

	template<typename T>
	size_t erase(KeyEx<T, KeyData> & key);

  size_t erase(IdType & key);

  void clear();

private:

	AnyMap myAnyMap;
};

// IMPLEMENTATION /////////////////////////////////////

template <class KeyData>
HeterogeneousMapEx<KeyData>::HeterogeneousMapEx(const HeterogeneousMapEx<KeyData> & toCopy)
{
  myAnyMap.insert(toCopy.myAnyMap.begin(), toCopy.myAnyMap.end());
  // Now tell all the keys that they have been inserted into the map
  BOOST_FOREACH(AnyMap::value_type & value, myAnyMap)
  {
    value.first->insertedIntoMap(*this);
  }
}

template <class KeyData>
HeterogeneousMapEx<KeyData>::~HeterogeneousMapEx()
{
  clear();
}

template <class KeyData>
bool HeterogeneousMapEx<KeyData>::empty() const
{
  return myAnyMap.empty();
}

template <class KeyData>
size_t HeterogeneousMapEx<KeyData>::size() const
{
  return myAnyMap.size();
}

template <class KeyData>
size_t HeterogeneousMapEx<KeyData>::max_size() const
{
  return myAnyMap.max_size();
}

template <class KeyData>
void HeterogeneousMapEx<KeyData>::clear()
{
  // Tell all the keys that they are being removed from the map
  BOOST_FOREACH(AnyMap::value_type & value, myAnyMap)
  {
    value.first->removedFromMap(*this);
  }

  myAnyMap.clear();
}

template <class KeyData>
size_t HeterogeneousMapEx<KeyData>::erase(KeyIdEx<KeyData> & key)
{
  const AnyMap::iterator it = myAnyMap.find(&key);

  if(it == myAnyMap.end())
    return 0;

  myAnyMap.erase(it);
  key.removedFromMap(*this);
  return 1;
}

template <class KeyData>
template <typename T>
bool HeterogeneousMapEx<KeyData>::insert(KeyEx<T, KeyData> & key, T value)
{
  ::std::pair<AnyMap::iterator, bool> result = myAnyMap.insert(AnyMap::value_type(key.getId(), value));

  if(result.second)
  {
    // Tell the key that it now stores a value in this map
    key.getId()->insertedIntoMap(*this);
  }

	return result.second;
}

template <class KeyData>
template <typename T>
bool HeterogeneousMapEx<KeyData>::insert(value_type<T> & x)
{
  ::std::pair<AnyMap::iterator, bool> result = myAnyMap.insert(AnyMap::value_type(x.first.getId(), x.second));

  if(result.second)
  {
    // Tell the key that it now stores a value in this map
    x.first.insertedIntoMap(*this);
  }

  return result.second;
}

template <class KeyData>
template <typename T>
T & HeterogeneousMapEx<KeyData>::operator [](KeyEx<T, KeyData> & key)
{
  T * value = find(key);

  if(!value)
  {
    // Insert default value
    ::std::pair<AnyMap::iterator, bool> result = myAnyMap.insert(AnyMap::value_type(key.getId(), T()));
    value = ::boost::any_cast<T>(&result.first->second);
    // Tell the key that it now stores a value in this map
    key.getId()->insertedIntoMap(*this);
  }

  return *value;
}

template <class KeyData>
template <typename T>
T * HeterogeneousMapEx<KeyData>::find(const KeyEx<T, KeyData> & key)
{
  // NOTE: Have to use const_cast<> here as we have a type of const KeyId * but the
  // map key is actually of type KeyId *.  In this case the cast is safe to do as we
  // guarantee that we will not modify the key.
	const AnyMap::iterator it = myAnyMap.find(const_cast<KeyData *>(key.getId()));

	if(it == myAnyMap.end())
		return NULL;

	return ::boost::any_cast<T>(&it->second);
}

template <class KeyData>
template <typename T>
const T * HeterogeneousMapEx<KeyData>::find(const KeyEx<T, KeyData> & key) const
{
  // NOTE: Have to use const_cast<> here as we have a type of const KeyId * but the
  // map key is actually of type KeyId *.  In this case the cast is safe to do as we
  // guarantee that we will not modify the key.
	const AnyMap::const_iterator it = myAnyMap.find(const_cast<KeyData *>(key.getId()));

	if(it == myAnyMap.end())
		return NULL;

	return ::boost::any_cast<T>(&it->second);
}

template <class KeyData>
template <typename T>
size_t HeterogeneousMapEx<KeyData>::erase(KeyEx<T, KeyData> & key)
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

#endif /* HETEROGENEOUS_MAP_EX_H */
