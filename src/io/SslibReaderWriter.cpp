/*
 * ResReaderWriter.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "io/SslibReaderWriter.h"

#include <iomanip>
#include <set>
#include <vector>

#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include <armadillo>

#include <yaml-cpp/yaml.h>

#include "common/Atom.h"
#include "common/AtomSpeciesDatabase.h"
#include "common/AtomSpeciesId.h"
#include "common/AtomSpeciesInfo.h"
#include "common/Structure.h"
#include "common/StructureProperties.h"
#include "common/Types.h"
#include "common/UnitCell.h"
#include "factory/SsLibYamlKeywords.h"
#include "io/IoFunctions.h"
#include "io/BoostFilesystem.h"
#include "io/StructureYamlGenerator.h"
#include "utility/IndexingEnums.h"

// DEFINES /////////////////////////////////


// NAMESPACES ////////////////////////////////


namespace sstbx {
namespace io {

namespace fs = ::boost::filesystem;
namespace properties = common::structure_properties;
namespace kw = factory::sslib_yaml_keywords;

const unsigned int SslibReaderWriter::DIGITS_AFTER_DECIMAL = 8;

void SslibReaderWriter::writeStructure(
	common::Structure & str,
	const ::boost::filesystem::path & filepath,
	const common::AtomSpeciesDatabase & speciesDb) const
{
	if(!filepath.has_filename())
		throw "Cannot write out structure without filepath";

  const io::StructureYamlGenerator generator(speciesDb);

  const YAML::Node structureNode = generator.generateNode(str);

  const fs::path dir = filepath.parent_path();
	if(!dir.empty() && !exists(dir))
	{
		create_directories(dir);
	}

  fs::ofstream strFile;
  strFile.open(filepath, ::std::ios_base::out | ::std::ios_base::app);

  if(strFile.is_open())
  {
    strFile << "---" << ::std::endl;
    strFile << YAML::Dump(structureNode) << ::std::endl;
    strFile << "..." << ::std::endl;
    strFile.close();
  }
}

common::types::StructurePtr SslibReaderWriter::readStructure(
	const ::boost::filesystem::path & filepath,
	const common::AtomSpeciesDatabase & speciesDb) const
{
  common::types::StructurePtr structure;
  const io::StructureYamlGenerator generator(speciesDb);

  ::std::vector<YAML::Node> docs = YAML::LoadAll(filepath.string());

  // Get the first structure found in the file
  BOOST_FOREACH(const YAML::Node & doc, docs)
  {
    if(doc[kw::STRUCTURE])
    {
      structure = generator.generateStructure(doc[kw::STRUCTURE]);

      if(structure.get())
        break;
    }
  }

  return structure;
}

size_t SslibReaderWriter::readStructures(
  StructuresContainer & outStructures,
	const ::boost::filesystem::path & filepath,
	const common::AtomSpeciesDatabase & speciesDb) const
{
  common::types::StructurePtr structure;
  const io::StructureYamlGenerator generator(speciesDb);

  size_t numLoaded = 0;
  ::std::vector<YAML::Node> docs = YAML::LoadAll(filepath.string());
  BOOST_FOREACH(const YAML::Node & doc, docs)
  {
    if(doc[kw::STRUCTURE])
    {
      structure = generator.generateStructure(doc[kw::STRUCTURE]);

      if(structure.get())
      {
        outStructures.push_back(structure.release());
        ++numLoaded;
      }
    }
  }

  return numLoaded;
}

::std::vector<std::string> SslibReaderWriter::getSupportedFileExtensions() const
{
  ::std::vector< ::std::string> exts;
  exts.push_back("sslib");
  return exts;
}

}
}
