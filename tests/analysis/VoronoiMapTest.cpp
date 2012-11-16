/*
 * VoronoiMapTest.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "sslibtest.h"

#include "analysis/VoronoiMap.h"

#include <SSLib.h>

#ifdef SSLIB_USE_CGAL

BOOST_AUTO_TEST_CASE(VoronoiMapTest)
{
  ::sstbx::analysis::VoronoiMap map;
}

#endif /* SSLIB_USE_CGAL */
