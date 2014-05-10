/*
 * SplReaderWriter.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "spl/io/SplReaderWriter.h"

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

#include "spl/common/Atom.h"
#include "spl/common/AtomSpeciesDatabase.h"
#include "spl/common/AtomSpeciesId.h"
#include "spl/common/AtomSpeciesInfo.h"
#include "spl/common/Structure.h"
#include "spl/common/StructureProperties.h"
#include "spl/common/Types.h"
#include "spl/common/UnitCell.h"
#include "spl/io/IoFunctions.h"
#include "spl/io/BoostFilesystem.h"
#include "spl/io/StructureSchema.h"
#include "spl/io/StructureYamlGenerator.h"
#include "spl/utility/IndexingEnums.h"
#include "spl/utility/UtilFunctions.h"

// DEFINES /////////////////////////////////

// NAMESPACES ////////////////////////////////

namespace spl {
namespace io {

namespace fs = boost::filesystem;
namespace properties = common::structure_properties;

const unsigned int SplReaderWriter::DIGITS_AFTER_DECIMAL = 8;
const std::string SplReaderWriter::DEFAULT_EXTENSION = "yaml";

namespace kw {
static const std::string STRUCTURE = "structure";
static const std::string STRUCTURES = "structures";
}

static const StructureSchema STRUCTURE_SCHEMA;

void
SplReaderWriter::writeStructure(common::Structure & str,
    const ResourceLocator & locator) const
{
  const fs::path filepath(locator.path());
  if(!filepath.has_filename())
    throw "Cannot write out structure without filepath";

  const io::StructureYamlGenerator generator;

  const fs::path dir = filepath.parent_path();
  if(!dir.empty() && !exists(dir))
    create_directories(dir);

  // First open and parse the file to get the current contents (if any)
  YAML::Node doc;
  fs::fstream strFile;
  if(fs::exists(filepath))
  {
    strFile.open(filepath, std::ios_base::in | std::ios_base::out);
    try
    {
      doc = YAML::Load(strFile);
    }
    catch(const YAML::Exception & /*e*/)
    {
      // The file is dodgy, so happily overwrite it
    }
    // Go back to the start of the file
    strFile.clear(); // Clear the EoF flag
    strFile.seekg(0, std::ios::beg);
  }
  else
  {
    strFile.open(filepath, std::ios_base::out);
  }

  ResourceLocator uniqueLoc = locator;
  if(uniqueLoc.id().empty())
  {
    std::string newId = str.getName();
    if(newId.empty())
    {
      newId = utility::generateUniqueName();
    }
    uniqueLoc.setId(newId);
  }

  const Structure structureInfo = generator.generateInfo(str);
  YAML::Node strNode = doc[kw::STRUCTURES][uniqueLoc.id()];
  STRUCTURE_SCHEMA.valueToNode(structureInfo, &strNode);

  if(strFile.is_open())
  {
    YAML::Emitter out;
    out << doc;
    strFile << out.c_str() << std::endl;
    strFile.close();

    str.setProperty(properties::io::LAST_ABS_FILE_PATH, uniqueLoc);
  }
}

common::types::StructurePtr
SplReaderWriter::readStructure(const ResourceLocator & locator) const
{
  common::types::StructurePtr structure;
  const io::StructureYamlGenerator generator;

  const fs::path filepath(locator.path());
  YAML::Node doc;
  try
  {
    doc = YAML::LoadFile(filepath.string());
  }
  catch(const YAML::Exception & /*e*/)
  {
    // Couldn't read the file
    return structure;
  }

  std::string locatorId = locator.id();
  if(locatorId.empty())
  {
    // No id, so assume that the file contains only one structure and
    // get the id
    if(!doc[kw::STRUCTURE])
      return structure;

    locatorId = doc[kw::STRUCTURE].begin()->first.as< std::string>();
  }

  if(!locatorId.empty())
  {
    if(doc[kw::STRUCTURE][locatorId])
    {
      Structure structureInfo;
      if(STRUCTURE_SCHEMA.nodeToValue(doc[kw::STRUCTURE][locatorId],
          &structureInfo))
        structure = generator.generateStructure(structureInfo);
    }
    else if(doc[kw::STRUCTURES][locatorId])
    {
      Structure structureInfo;
      if(STRUCTURE_SCHEMA.nodeToValue(doc[kw::STRUCTURES][locatorId],
          &structureInfo))
        structure = generator.generateStructure(structureInfo);
    }
  }

  if(structure.get())
  {
    structure->setProperty(properties::io::LAST_ABS_FILE_PATH,
        ResourceLocator(io::absolute(filepath), locatorId));
  }

  return structure;
}

size_t
SplReaderWriter::readStructures(StructuresContainer & outStructures,
    const ResourceLocator & locator) const
{
  size_t numLoaded = 0;
  if(locator.id().empty())
  {
    const io::StructureYamlGenerator generator;

    const fs::path filepath(locator.path());
    YAML::Node doc;
    try
    {
      doc = YAML::LoadFile(filepath.string());
    }
    catch(const YAML::Exception & /*e*/)
    {
      return 0;
    }

    common::types::StructurePtr structure;
    std::string structureId;

    if(doc[kw::STRUCTURE] && doc[kw::STRUCTURE].IsMap())
    {
      const YAML::Node::const_iterator it = doc[kw::STRUCTURE].begin();
      structureId = it->first.as< std::string>();
      Structure structureInfo;
      if(STRUCTURE_SCHEMA.nodeToValue(it->second, &structureInfo))
        structure = generator.generateStructure(structureInfo);

      if(structure.get())
      {
        structure->setProperty(properties::io::LAST_ABS_FILE_PATH,
            ResourceLocator(io::absolute(filepath), structureId));
        outStructures.push_back(structure.release());
        ++numLoaded;
      }
    }

    if(doc[kw::STRUCTURES] && doc[kw::STRUCTURES].IsMap())
    {
      for(YAML::const_iterator it = doc[kw::STRUCTURES].begin(), end =
          doc[kw::STRUCTURES].end(); it != end; ++it)
      {
        structureId = it->first.as< std::string>();
        Structure structureInfo;
        if(STRUCTURE_SCHEMA.nodeToValue(it->second, &structureInfo))
          structure = generator.generateStructure(structureInfo);

        if(structure.get())
        {
          structure->setProperty(properties::io::LAST_ABS_FILE_PATH,
              ResourceLocator(io::absolute(filepath), structureId));
          outStructures.push_back(structure.release());
          ++numLoaded;
        }
      }
    }
  }
  else
  {
    // The user has specified a particular id so they only want one structure
    common::types::StructurePtr structure = readStructure(locator);
    if(structure.get())
    {
      outStructures.push_back(structure.release());
      ++numLoaded;
    }
  }

  return numLoaded;
}

::std::vector< std::string>
SplReaderWriter::getSupportedFileExtensions() const
{
  std::vector< std::string> exts;
  exts.push_back("yaml");
  exts.push_back("spl");
  exts.push_back("sslib");
  return exts;
}

bool
SplReaderWriter::multiStructureSupport() const
{
  return true;
}

}
}
