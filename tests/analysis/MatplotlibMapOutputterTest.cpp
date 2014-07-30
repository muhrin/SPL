/*
 * MatplotlibMapOutputterTest.cpp
 *
 *  Created on: Jun 27, 2014
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "sslibtest.h"

#include <vector>

#include <spl/analysis/MapArrangementTraits.h>
#include <spl/analysis/MatplotlibMapOutputter.h>
#include <spl/analysis/VoronoiPathTracer.h>
#include <spl/math/Random.h>

using namespace spl;

BOOST_AUTO_TEST_SUITE(MatplotlibMapOutputter)

typedef analysis::MapArrangementTraits< int> MapTraits;
typedef analysis::VoronoiPathTracer< MapTraits> Tracer;
typedef MapTraits::Arrangement Map;

BOOST_AUTO_TEST_CASE(SimpleTest)
{
  // SETTINGS
  static const size_t NUM_POINTS = 100;

  // Create a bunch of points at random positions with random labels
  std::vector< Tracer::PointLabel> points;
  for(size_t i = 0; i < NUM_POINTS; ++i)
    points.push_back(
        std::make_pair(
            Tracer::Point(math::randu< double>(), math::randu< double>()),
            math::randu< int>(0, 2)));

  const Map map = Tracer::processPath(points.begin(), points.end());
  BOOST_CHECK(map.is_valid());

  analysis::MatplotlibMapOutputter< MapTraits> outputter;
  outputter.outputArrangement(map, &std::cout);
}

BOOST_AUTO_TEST_SUITE_END()
