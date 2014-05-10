/*
 * XyzReaderWriter.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "spl/io/XyzReaderWriter.h"

#include <iomanip>
#include <set>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include <armadillo>

#include "spl/common/Atom.h"
#include "spl/common/AtomSpeciesDatabase.h"
#include "spl/common/AtomSpeciesId.h"
#include "spl/common/AtomSpeciesInfo.h"
#include "spl/common/Structure.h"
#include "spl/common/StructureProperties.h"
#include "spl/common/Types.h"
#include "spl/common/UnitCell.h"
#include "spl/io/InfoLine.h"
#include "spl/io/IoFunctions.h"
#include "spl/io/BoostFilesystem.h"
#include "spl/utility/IndexingEnums.h"

// DEFINES /////////////////////////////////

// NAMESPACES ////////////////////////////////

namespace spl {
namespace io {

namespace fs = boost::filesystem;
namespace ssc = spl::common;
namespace properties = ssc::structure_properties;

typedef boost::tokenizer< boost::char_separator< char> > Tok;

// Set up our tokenizer to split around space and tab
static const boost::char_separator< char> SEP(" \t");

const unsigned int XyzReaderWriter::DIGITS_AFTER_DECIMAL = 8;

void
XyzReaderWriter::writeStructure(spl::common::Structure & str,
    const ResourceLocator & locator) const
{
  using namespace utility::cell_params_enum;
  using namespace utility::cart_coords_enum;
  using spl::common::AtomSpeciesId;

  const fs::path filepath(locator.path());
  if(!filepath.has_filename())
    throw "Cannot write out structure without filepath";

  const fs::path dir = filepath.parent_path();
  if(!dir.empty() && !exists(dir))
    create_directories(dir);

  fs::ofstream strFile;
  strFile.open(filepath);

  // Number of atoms
  strFile << str.getNumAtoms() << "\n";

  //////////////////////////
  // Start Title
  InfoLine infoLine(str);
  if(!infoLine.name || infoLine.name->empty())
    infoLine.name = filepath.string();
  strFile << infoLine << "\n";
  // End title //////////////////

  ////////////////////////////
  // Start atoms

  // Now write out the atom positions along with the species
  for(size_t i = 0; i < str.getNumAtoms(); ++i)
  {
    const common::Atom & atom = str.getAtom(i);
    const arma::vec3 & pos = atom.getPosition();

    if(!atom.getSpecies().empty())
      strFile << atom.getSpecies() << " ";
    else
      strFile << "DU ";
    strFile << std::setprecision(12) << pos(X) << " " << pos(Y) << " " << pos(Z)
        << "\n";
  }
  // End atoms ///////////

  str.setProperty(properties::io::LAST_ABS_FILE_PATH,
      io::ResourceLocator(io::absolute(filepath)));

  if(strFile.is_open())
    strFile.close();
}

common::types::StructurePtr
XyzReaderWriter::readStructure(const ResourceLocator & resourceLocator) const
{
  common::types::StructurePtr str;
  if(resourceLocator.empty() || !fs::exists(resourceLocator.path()))
    return str;

  fs::ifstream is(resourceLocator.path());
  if(!is.is_open())
    return str;

  str.reset(new common::Structure());
  std::string line;

  // Skip the first line: just contains num atoms
  std::getline(is, line);

  // Title line
  InfoLine infoLine;
  is >> infoLine;
  infoLine.populate(str.get());

  // Atoms
  readAtoms(&is, str.get());

  return str;
}

size_t
XyzReaderWriter::readStructures(StructuresContainer & outStructures,
    const ResourceLocator & resourceLocator) const
{
  common::types::StructurePtr str = readStructure(resourceLocator);
  if(str.get())
    outStructures.push_back(str);
  return 1;
}

std::vector< std::string>
XyzReaderWriter::getSupportedFileExtensions() const
{
  std::vector< std::string> ext;
  ext.push_back("xyz");
  return ext;
}

bool
XyzReaderWriter::multiStructureSupport() const
{
  return false;
}

void
XyzReaderWriter::readAtoms(std::istream * const is,
    common::Structure * const structure) const
{
  std::string line;
  std::string species;
  arma::vec3 pos;
  while(std::getline(*is, line))
  {
    const Tok tok(line, SEP);
    Tok::const_iterator it = tok.begin();
    const Tok::const_iterator end = tok.end();

    if(it == end)
      continue;
    species = *it;

    try
    {
      if(++it == end)
        continue;
      pos(0) = boost::lexical_cast<double>(*it);
      if(++it == end)
        continue;
      pos(1) = boost::lexical_cast<double>(*it);
      if(++it == end)
        continue;
      pos(2) = boost::lexical_cast<double>(*it);
    }
    catch(const boost::bad_lexical_cast & /*e*/)
    {
      continue;
    }
    structure->newAtom(species).setPosition(pos);
  }
}

}
}
