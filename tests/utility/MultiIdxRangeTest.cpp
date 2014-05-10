/*
 * MultiIdxRangeTest.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "sslibtest.h"

#include <iostream>

#include <boost/foreach.hpp>

#include <spl/utility/MultiIdx.h>
#include <spl/utility/MultiIdxRange.h>

namespace ssu = ::spl::utility;

BOOST_AUTO_TEST_CASE(SimpleRange)
{
  // SETTINGS //////////////
  const size_t dims = 2;
  const size_t startNum = 0;
  const size_t endNum = 3;

  ssu::MultiIdx<unsigned int> x0(dims), x1(dims);
  
  x0.fill(startNum); // Place start at 0,0
  x1.fill(endNum); // Place end at 3,3

  ssu::MultiIdxRange<unsigned int> range(x0, x1);

  size_t i = 0;
  for(ssu::MultiIdxRange<unsigned int>::const_iterator it = range.begin(), end = range.end();
    it != end; ++it)
  {
    BOOST_CHECK(i % 3 == (*it)[0]);
    ++i;
  }
  BOOST_CHECK(i == (x1 - x0).product());

  // Check in reverse, i should be at correct point
  //++i;
  //for(ssu::MultiIdxRange<unsigned int>::const_iterator it = range.end(), end = range.begin();
  //  it != end; --it)
  //{
  //  BOOST_CHECK(i % 3 == (*it)[0]);
  //  --i;
  //}
}

BOOST_AUTO_TEST_CASE(OneDimensionalRange)
{
  // SETTINGS //////////////
  const int startNum = 2;
  const int endNum = 207;

  ssu::MultiIdx<unsigned int> x0(1), x1(1);
  
  x0.fill(startNum); // Place start at 0
  x1.fill(endNum); // Place end at 41

  const ssu::MultiIdxRange<unsigned int> range(x0, x1);

  BOOST_CHECK((endNum - startNum) == (x1 - x0).product());

  size_t i = startNum;
  for(ssu::MultiIdxRange<unsigned int>::const_iterator it = range.begin(), end = range.end();
    it != end; ++it)
  {
    // Make sure each entry is correct
    BOOST_CHECK(i == (*it)[0]);
    ++i;
  }

  // Now try in reverse
  //i = endNum;
  //for(ssu::MultiIdxRange<unsigned int>::const_iterator it = range.end(), end = range.begin();
  //  it != end; --it)
  //{
  //  // Make sure each entry is correct
  //  BOOST_CHECK(i == (*it)[0]);
  //  --i;
  //}
}
