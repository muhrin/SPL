/*
 * HeterogeneousMap.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef HETEROGENEOUS_MAP_H
#define HETEROGENEOUS_MAP_H

// INCLUDES /////////////////////////////////////////////

#include <map>

#include <boost/any.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/utility.hpp>

#include <utility/HeterogeneousMapKey.h>


// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx {
namespace utility {

class HeterogeneousMap
{
  typedef ::std::map<KeyId *, ::boost::any> AnyMap;
public:

  template <typename Type>
  struct value_type : public ::std::pair<Key<Type>, Type>
  {};

  HeterogeneousMap() {}
  HeterogeneousMap(const HeterogeneousMap & toCopy);
  ~HeterogeneousMap();
 
  // Capacity methods
  bool empty() const;
  size_t size() const;
  size_t max_size() const;

	template <typename T>
	bool insert(Key<T> & key, T value);

  template <typename T>
  bool insert(value_type<T> & x);

  template <typename T>
  T & operator [](Key<T> & x);

	template<typename T>
	T * find(const Key<T> & key);

	template<typename T>
	const T * find(const Key<T> & key) const;

	template<typename T>
	size_t erase(Key<T> & key);

  size_t erase(KeyId & key);

  void clear();

private:

  size_t eraseNoNotify(KeyId & key);

	AnyMap myAnyMap;

  friend KeyId::~KeyId();
};

}
}

#include "utility/detail/HeterogeneousMap.h"

#endif /* HETEROGENEOUS_MAP_H */
