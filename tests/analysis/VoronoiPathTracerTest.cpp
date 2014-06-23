/*
 * VoronoiPathTracerTest.cpp
 *
 *  Created on: Jun 19, 2014
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "sslibtest.h"

#include <vector>

#include <spl/analysis/MapArrangementTraits.h>
#include <spl/analysis/VoronoiPathTracer.h>

#include <spl/math/Random.h>

using namespace spl;

BOOST_AUTO_TEST_SUITE(VoronoiPathTracer)

BOOST_AUTO_TEST_CASE(SimpleTest)
{
  typedef analysis::MapArrangementTraits< int> MapTraits;
  typedef MapTraits::Arrangement Map;

  // SETTINGS
  static const size_t NUM_POINTS = 100;

  // Create a bunch of points at random positions with random labels
  std::vector< MapTraits::PointLabel> points;
  for(size_t i = 0; i < NUM_POINTS; ++i)
    points.push_back(
        std::make_pair(
            MapTraits::Point(math::randu< double>(), math::randu< double>()),
            math::randu< int>(0, 2)));

  const Map arr = analysis::processPath< MapTraits>(points.begin(),
      points.end());
}

BOOST_AUTO_TEST_SUITE_END()
