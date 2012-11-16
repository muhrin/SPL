/*
 * PermutationRangeTest.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "sslibtest.h"

#include <iostream>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/scoped_array.hpp>

#include <utility/PermutationRange.h>


namespace ssu = ::sstbx::utility;

BOOST_AUTO_TEST_CASE(HeapArrayIndexRemapping)
{
  // SETTINGS /////////////////
  typedef unsigned short DataType;
  const size_t numElements = 5;

  ::boost::scoped_array<DataType> sArray(new DataType[numElements]);

  ::std::vector<ptrdiff_t> mapping(numElements);

  for(size_t i = 0; i < numElements; ++i)
  {
    sArray[i] = i;
    mapping[i] = numElements - i - 1;
  }

  DataType * tmpPtr = sArray.get();
  ssu::IndexRemappingViewShared<DataType *, DataType> remapper(tmpPtr, mapping);
  ssu::IndexRemappingViewShared<DataType *, DataType> remapperCopy(remapper);

  for(size_t i = 0; i < numElements; ++i)
  {
    BOOST_REQUIRE(remapper[i] == numElements - i - 1);
    BOOST_REQUIRE(remapperCopy[i] == numElements - i - 1);
  }

  // Try a different mapping
  size_t newIdx;
  for(size_t i = 0; i < numElements; ++i)
  {
    newIdx = (i + 7) % numElements;
    mapping[i] = newIdx;
    BOOST_REQUIRE(remapper[i] == (i + 7) % numElements);
    BOOST_REQUIRE(remapperCopy[i] == (i + 7) % numElements);
  }
}

BOOST_AUTO_TEST_CASE(StdVectorIndexRemapping)
{
  // SETTINGS /////////////////
  typedef unsigned short DataType;
  typedef ::std::vector<DataType> CollectionType;
  const size_t numElements = 5;

  CollectionType collection(numElements);

  ::std::vector<ptrdiff_t> mapping(numElements);

  // Collection: i..numElements forwards, mapping: backwards
  for(size_t i = 0; i < numElements; ++i)
  {
    collection[i] = i;
    mapping[i] = numElements - i - 1;
  }

  ssu::IndexRemappingViewShared<CollectionType, DataType> remapper(collection, mapping);
  ssu::IndexRemappingViewShared<CollectionType, DataType> remapperCopy(remapper);

  for(size_t i = 0; i < numElements; ++i)
  {
    BOOST_REQUIRE(remapper[i] == numElements - i - 1);
    BOOST_REQUIRE(remapperCopy[i] == numElements - i - 1);
  }

  // Try a different mapping
  size_t newIdx;
  for(size_t i = 0; i < numElements; ++i)
  {
    newIdx = (i + 7) % numElements;
    mapping[i] = newIdx;
    BOOST_REQUIRE(remapper[i] == (i + 7) % numElements);
    BOOST_REQUIRE(remapperCopy[i] == (i + 7) % numElements);
  }
}

BOOST_AUTO_TEST_CASE(VectorTest)
{
  // SETTINGS ///////
  typedef unsigned short DataType;
  typedef ::std::vector<DataType> CollectionType;
  typedef ssu::PermutationRange<CollectionType, DataType> Range;
  const size_t maxNumElements = 5;


  CollectionType collection;

  long unsigned int numPerms, totalPerms = 1;
  for(size_t i = 1; i < maxNumElements; ++i)
  {
    collection.push_back(i);

    Range permutations(collection);

    totalPerms *= i;
    numPerms = 0;
    BOOST_FOREACH(const Range::iterator_t::value_type & permutation, permutations)
    {
      ++numPerms;
    }
    BOOST_REQUIRE(numPerms == totalPerms);
  }
}

BOOST_AUTO_TEST_CASE(ArrayPointerTest)
{
  // SETTINGS ///////
  const size_t shortArrayNumElements = 3;

  unsigned short * sArray = new unsigned short[shortArrayNumElements];

  for(size_t i = 0; i < shortArrayNumElements; ++i)
  {
    sArray[i] = i;
  }

  ssu::PermutationRange<const unsigned short *, const unsigned short> numRange(sArray, shortArrayNumElements);

  for(ssu::PermutationRange<const unsigned short *, const unsigned short>::iterator_t it = numRange.begin(),
    end = numRange.end(); it != end; ++it)
  {
    ::std::cout << "Permutation" << ::std::endl;

    for(size_t i = 0; i < shortArrayNumElements; ++i)
    {
      ::std::cout << " " << (*it)[i];
    }

    ::std::cout << ::std::endl;
  }


  delete[] sArray;
}
