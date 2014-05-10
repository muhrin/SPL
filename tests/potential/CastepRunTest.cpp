/*
 * CastepRunTest.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "sslibtest.h"

#include <boost/foreach.hpp>

#include <spl/io/CastepReader.h>
#include <spl/io/CellReaderWriter.h>
#include <spl/potential/CastepRun.h>

// NAMESPACES ///////////////////////////////
namespace ssio = spl::io;
namespace ssp = spl::potential;

BOOST_AUTO_TEST_CASE(CastepRunTest)
{
  std::vector< std::string> SEEDS;
  SEEDS.push_back("run1");
  SEEDS.push_back("run2");
  SEEDS.push_back("run3");
  SEEDS.push_back("run4");

  ssio::CastepReader castepReader;
  ssio::CellReaderWriter cellReader;

  for(size_t i = 0; i < 3; ++i)
  {
    ssp::CastepRun castepRun(SEEDS[i], cellReader, castepReader);
    BOOST_CHECK(castepRun.finishedSuccessfully());
  }
  for(size_t i = 3; i < 4; ++i)
  {
    ssp::CastepRun castepRun(SEEDS[i], cellReader, castepReader);
    BOOST_CHECK(!castepRun.finishedSuccessfully());
  }
}
