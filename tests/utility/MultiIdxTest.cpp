/*
 * MultiIdxTest.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "sslibtest.h"

#include <iostream>

#include <boost/foreach.hpp>

#include <utility/MultiIdx.h>

namespace ssu = ::sstbx::utility;

BOOST_AUTO_TEST_CASE(BasicTest)
{
  const size_t dims = 3;
  ssu::MultiIdx<unsigned int> a(dims), b(dims);

  BOOST_CHECK(a.dims() == dims);

  for(size_t i = 0; i < dims; ++i)
  {
    BOOST_CHECK(a[i] == 0);
    BOOST_CHECK(a[i] == b[i]);
  }
  BOOST_CHECK(a == b);

  b.fill(3);

  for(size_t i = 0; i < dims; ++i)
  {
    BOOST_CHECK(b[i] == 3);
  }
  BOOST_CHECK(a != b);
  BOOST_CHECK(b > a);

  BOOST_CHECK(b.sum() == 3 * dims);
  BOOST_CHECK(b.product() == (long unsigned int)::std::pow((double)3, (double)dims));


}
