/*
 * YamlFactoryTest.cpp
 *
 *  Created on: Oct 3, 2012
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "sslibtest.h"

#include <iostream>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem.hpp>

#include <spl/build_cell/BuildCellFwd.h>
#include <spl/factory/FactoryError.h>
#include <spl/factory/SsLibFactoryYaml.h>
#include <spl/factory/SsLibYamlSchema.h>

using namespace spl;

BOOST_AUTO_TEST_SUITE(YamlFactory)

BOOST_AUTO_TEST_CASE(StructureGenerator)
{
  //// Settings ////////////////
  const char simpleStructure[] = "RandomStructure.yaml";

  factory::Factory factory;

  BOOST_REQUIRE(boost::filesystem::exists(boost::filesystem::path(simpleStructure)));
  const YAML::Node loadedNode = YAML::LoadFile(simpleStructure);
  BOOST_REQUIRE(loadedNode.IsDefined());

  factory::builder::StructureGeneratorSchema schema;
  factory::builder::StructureGenerator generator;
  schemer::ParseLog log;
  BOOST_REQUIRE(schema.nodeToValue(loadedNode, &generator, &log));
  log.printErrors();
  BOOST_REQUIRE(!log.hasErrors());

  try
  {
    build_cell::IStructureGeneratorPtr strGen = factory.createStructureGenerator(generator);
    BOOST_CHECK(strGen.get());
  }
  catch(const factory::FactoryError & e)
  {
    std::cerr << boost::diagnostic_information(e) << std::endl;
  }
}

BOOST_AUTO_TEST_SUITE_END()
