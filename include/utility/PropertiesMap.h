/*
 * PropertiesMap.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef PROPERTIES_MAP_H
#define PROPERTIES_MAP_H

// INCLUDES /////////////////////////////////////////////

#include <boost/any.hpp>
#include <boost/utility.hpp>

#include <map>

// FORWARD DECLARATIONS ////////////////////////////////////

namespace sstbx { namespace utility {

struct KeySupertype : ::boost::noncopyable
{
protected:
	// Can't create an instance of this type
	KeySupertype() {}
};

template <typename Type>
struct Key : public KeySupertype
{
public:
	Key() {}
};


class PropertiesMap
{
public:

	template<typename T>
	::std::pair<T * const, bool> insert(const Key<T> & key, T value);

	template<typename T>
	T * find(Key<T> & key);

	template<typename T>
	const T * find(const Key<T> & key) const;

	template<typename T>
	void erase(const Key<T> & key);

private:

	typedef ::std::map<const KeySupertype *, ::boost::any> AnyMap;

	typedef ::std::pair<const KeySupertype *, ::boost::any> AnyMapPair;

	AnyMap myAnyMap;
};

// IMPLEMENTATION /////////////////////////////////////

template <typename T>
::std::pair<T * const, bool> PropertiesMap::insert(const Key<T> & key, T value)
{
	using ::std::pair;

	pair<AnyMap::iterator, bool> result = myAnyMap.insert(AnyMapPair(&key, value));

	T * const out =	result.second ?
		&value :
		::boost::any_cast<T>(&result.first->second);

	return pair<T * const, bool>(out, result.second);
}

template <typename T>
T * PropertiesMap::find(Key<T> & key)
{
	const AnyMap::iterator it = myAnyMap.find(&key);

	if(it == myAnyMap.end())
		return NULL;

	return ::boost::any_cast<T>(&it->second);
}

template <typename T>
const T * PropertiesMap::find(const Key<T> & key) const
{
	const AnyMap::const_iterator it = myAnyMap.find(&key);

	if(it == myAnyMap.end())
		return NULL;

	return ::boost::any_cast<T>(&it->second);
}

template <typename T>
void PropertiesMap::erase(const Key<T> & key)
{
	const AnyMap::iterator it = myAnyMap.find(&key);

	if(it == myAnyMap.end())
		return;

	myAnyMap.erase(it);
}

}}

#endif /* PROPERTIES_MAP_H */
