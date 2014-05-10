/*
 * HeterogeneousMapTest.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include <iostream>
#include <string>

#include <boost/test/unit_test.hpp>

#include <spl/utility/HeterogeneousMap.h>
#include <spl/utility/HeterogeneousMapEx.h>

namespace ssu = ::spl::utility;

static ssu::Key<int> INT_KEY;
static ssu::Key< ::std::string> STRING_KEY;

BOOST_AUTO_TEST_CASE(HeterogeneousMapTest)
{
  ssu::Key<int> INT_KEY;
  ssu::Key< ::std::string> STRING_KEY;

  // SETTINGS //////////////
  const int intVal = 255;
  const int intVal2 = 14124;
  const ::std::string stringVal("Hello heterogeneous map!");

  const ssu::Key<int> testConstKey;

  ssu::Key<int> intKeyCopy = INT_KEY;
  int * intValPtr;

  ssu::HeterogeneousMap map1;

  map1[INT_KEY] = intVal;

  map1.find(testConstKey);

  intValPtr = map1.find(INT_KEY);

  BOOST_REQUIRE(intValPtr && *intValPtr == intVal);

  intValPtr = map1.find(intKeyCopy);
  BOOST_REQUIRE(intValPtr && *intValPtr == intVal);

  map1[intKeyCopy] = intVal2;

  intValPtr = map1.find(INT_KEY);
  BOOST_REQUIRE(intValPtr && *intValPtr == intVal2);

  *intValPtr = intVal;

  intValPtr = map1.find(INT_KEY);
  BOOST_REQUIRE(intValPtr && *intValPtr == intVal);

  const size_t mapSizeBefore = map1.size();
  {
    ssu::Key<double> scopedDoubleKey;
    ssu::Key<int> scopedIntKeyCopy = INT_KEY;
    map1[scopedDoubleKey] = 127.0;
    BOOST_REQUIRE(map1.size() == mapSizeBefore + 1);
  }
  // Make sure that the key going out of scope has
  // removed the entry from the map
  BOOST_REQUIRE(map1.size() == mapSizeBefore);

  // Now check that if the map goes out of scope the
  // key is updated correctly
  {
    ssu::HeterogeneousMap scopedMap;

    scopedMap[INT_KEY] = intVal;
  }

  // Now try clearing the map
  map1.clear();
  BOOST_REQUIRE(map1.size() == 0);

  intValPtr = map1.find(INT_KEY);
  BOOST_REQUIRE(!intValPtr);

  intValPtr = map1.find(intKeyCopy);
  BOOST_REQUIRE(!intValPtr);

  // Now try erase
  map1[INT_KEY] = intVal;
  BOOST_REQUIRE(map1.size() == 1);
  BOOST_REQUIRE(map1.find(INT_KEY));

  map1.erase(INT_KEY);
  BOOST_REQUIRE(map1.size() == 0);
  BOOST_REQUIRE(!map1.find(INT_KEY));

  // Currently not testing:
  // const T * HeterogeneousMap::find(Key<T> & key) const
  // bool HeterogeneousMap::insert(Key<T> & key, T value)
}

template <typename T>
struct NamedKey
{
  typedef ssu::KeyEx<T, ::std::string> Type;
};

BOOST_AUTO_TEST_CASE(HeterogeneousMapExTest)
{
  NamedKey<int>::Type AGE_KEY("age");

  ssu::HeterogeneousMapEx< ::std::string> map;

  map.insert(AGE_KEY, 15);
  map.erase(AGE_KEY);
}
