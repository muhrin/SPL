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

#include <yaml-cpp/yaml.h>

#include <build_cell/Types.h>
#include <common/AtomSpeciesDatabase.h>
#include <factory/FactoryError.h>
#include <factory/SsLibFactoryYaml.h>
#include <factory/SsLibYamlKeywords.h>

namespace ssbc = ::sstbx::build_cell;
namespace ssc  = ::sstbx::common;
namespace ssf = ::sstbx::factory;

namespace kw = ::sstbx::factory::sslib_yaml_keywords;

BOOST_AUTO_TEST_CASE(StructureDescriptionTest)
{
  // Settings ////////////////
  const char simpleStructure[] = "RandomStructure.sslib";

  ssc::AtomSpeciesDatabase speciesDb;

  ssf::SsLibFactoryYaml factory(speciesDb);

  YAML::Node loadedNode = YAML::LoadFile(simpleStructure);

  if(loadedNode[kw::RANDOM_STRUCTURE])
  {
    const YAML::Node & strNode = loadedNode[kw::RANDOM_STRUCTURE];

    try
    {
      ssbc::StructureDescriptionPtr strGen = factory.createStructureDescription(strNode);
    }
    catch(const ssf::FactoryError & e)
    {
      ::std::cout << ::boost::diagnostic_information(e) << ::std::endl;
    }
  }
}
