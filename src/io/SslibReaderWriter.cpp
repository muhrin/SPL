/*
 * ResReaderWriter.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "io/SslibReaderWriter.h"

#include <iostream>
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
	const ResourceLocator & locator,
	const common::AtomSpeciesDatabase & speciesDb) const
{
  const fs::path filepath(locator.path());
	if(!filepath.has_filename())
		throw "Cannot write out structure without filepath";

  const io::StructureYamlGenerator generator(speciesDb);

  const fs::path dir = filepath.parent_path();
	if(!dir.empty() && !exists(dir))
	{
		create_directories(dir);
	}

  // First open and parse the file to get the current contents (if any)
  YAML::Node doc;
  if(fs::exists(filepath))
    doc = YAML::LoadFile(filepath.string());

  ::std::string structureId = locator.id();

  // TODO: Need to generate a unique id if one isn't specified
  if(structureId.empty())
  {
    structureId = str.getName();
    if(structureId.empty())
      return;
  }

  doc[kw::STRUCTURES][structureId] = generator.generateNode(str);

  fs::ofstream strFile;
  strFile.open(filepath, ::std::ios_base::out);

  if(strFile.is_open())
  {
    strFile << YAML::Dump(doc) << ::std::endl;
    strFile.close();
  }
}

common::types::StructurePtr SslibReaderWriter::readStructure(
	const ResourceLocator & locator,
	const common::AtomSpeciesDatabase & speciesDb) const
{
  common::types::StructurePtr structure;
  const io::StructureYamlGenerator generator(speciesDb);

  const fs::path filepath(locator.path());
  YAML::Node doc = YAML::LoadFile(filepath.string());

  ::std::string locatorId = locator.id();
  if(locatorId.empty())
  {
    // No id, so assume that the file contains only one structure and
    // get the id
    if(!doc[kw::STRUCTURE])
      return structure;

    locatorId = doc[kw::STRUCTURE].begin()->first.as< ::std::string>();
  }

  if(!locatorId.empty())
  {
    if(doc[kw::STRUCTURE][locatorId])
      structure = generator.generateStructure(doc[kw::STRUCTURE][locatorId]);
    else if(doc[kw::STRUCTURES][locatorId])
      structure = generator.generateStructure(doc[kw::STRUCTURES][locatorId]);
  }

  if(structure.get())
  {
    structure->setProperty(
      properties::io::LAST_ABS_FILE_PATH,
      ResourceLocator(io::absolute(filepath), locatorId)
    );
  }

  return structure;
}

size_t SslibReaderWriter::readStructures(
  StructuresContainer & outStructures,
	const ResourceLocator & locator,
	const common::AtomSpeciesDatabase & speciesDb) const
{
  size_t numLoaded = 0;

  if(locator.id().empty())
  {
    const io::StructureYamlGenerator generator(speciesDb);

    const fs::path filepath(locator.path());
    const YAML::Node doc = YAML::LoadFile(filepath.string());

    common::types::StructurePtr structure;
    ::std::string structureId;

    if(doc[kw::STRUCTURE] && doc[kw::STRUCTURE].IsMap())
    {
      const YAML::Node::const_iterator strIt = doc[kw::STRUCTURE].begin();
      structureId = strIt->first.as< ::std::string>();
      structure = generator.generateStructure(strIt->second);

      if(structure.get())
      {
        structure->setProperty(
          properties::io::LAST_ABS_FILE_PATH,
          ResourceLocator(io::absolute(filepath), structureId)
        );
        outStructures.push_back(structure.release());
        ++numLoaded;
      }
    }

    if(doc[kw::STRUCTURES] && doc[kw::STRUCTURES].IsMap())
    {
      for(YAML::const_iterator it = doc[kw::STRUCTURES].begin(), end = doc[kw::STRUCTURES].end();
        it != end; ++it)
      {
        structureId = it->first.as< ::std::string>();
        structure = generator.generateStructure(it->second);

        if(structure.get())
        {
          structure->setProperty(
            properties::io::LAST_ABS_FILE_PATH,
            ResourceLocator(io::absolute(filepath), structureId)
          );
          outStructures.push_back(structure.release());
          ++numLoaded;
        }
      }
    }
  }
  else
  {
    // The user has specified a particular id so they only want one structure
    common::types::StructurePtr structure = readStructure(locator, speciesDb);

    if(structure.get())
      outStructures.push_back(structure.release());
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
