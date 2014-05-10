/*
 * CellReaderWriter.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "spl/io/CellReaderWriter.h"

#include <iomanip>
#include <limits>

#include <boost/algorithm/string/find.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/tokenizer.hpp>

#include "spl/common/AtomSpeciesDatabase.h"
#include "spl/common/Structure.h"
#include "spl/common/UnitCell.h"
#include "spl/io/BoostFilesystem.h"
#include "spl/io/Parsing.h"
#include "spl/utility/IndexingEnums.h"

// DEFINES /////////////////////////////////

// NAMESPACES ////////////////////////////////

namespace spl {
namespace io {

namespace fs = boost::filesystem;

std::vector< std::string>
CellReaderWriter::getSupportedFileExtensions() const
{
  return std::vector< std::string>(1, "cell");
}

common::types::StructurePtr
CellReaderWriter::readStructure(const ResourceLocator & locator) const
{
  common::types::StructurePtr structure;
  const fs::path filepath(locator.path());
  if(!filepath.has_filename())
    return structure; // Can't write out structure without filepath

  fs::ifstream strFile;
  strFile.open(filepath);

  structure = readStructure(strFile);

  if(strFile.is_open())
    strFile.close();

  return structure;
}

size_t
CellReaderWriter::readStructures(StructuresContainer & outStructures,
    const ResourceLocator & resourceLocator) const
{
  common::types::StructurePtr structure = readStructure(resourceLocator);
  if(structure.get())
  {
    outStructures.push_back(structure.release());
    return 1;
  }
  return 0;
}

common::types::StructurePtr
CellReaderWriter::readStructure(std::istream & is) const
{
  using namespace utility::cart_coords_enum;

  static const boost::regex RE_FLOAT(
      "([-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?)");
  typedef boost::tokenizer< boost::char_separator< char> > Tok;
  const boost::char_separator< char> sep(" \t");

  common::types::StructurePtr structure(new common::Structure());

  std::string line;
  boost::smatch match;
  if(findNextLine(line, is, "%BLOCK lattice_", false))
  {
    // Unit cell
    if(boost::ifind_first(line, "_abc"))
    { // abc format
      int offset = 0;
      while(std::getline(is, line) && !::boost::ifind_first(line, "%ENDBLOCK"))
      {
        if(boost::regex_search(line, match, RE_FLOAT)) // Have we reached numbers yet?
        {
          double latticeParams[6];

          Tok tok(line, sep);
          Tok::iterator tokIt = tok.begin();
          unsigned int i;
          for(i = 0; i < 3 && tokIt != tok.end(); ++i)
            latticeParams[i + offset] = boost::lexical_cast< double>(*tokIt++);

          // Did we get all three parts
          if(i < 3)
            break;
          // Have we got both 
          if(offset == 0)
            offset += 3;
          else
          {
            structure->setUnitCell(common::UnitCell(latticeParams));
            break;
          }
        }
      }
    }
    else if(boost::ifind_first(line, "_cart"))
    { // cart format
      int vec = 0;
      arma::mat33 mtx;
      while(std::getline(is, line) && !::boost::ifind_first(line, "%ENDBLOCK"))
      {
        if(boost::regex_search(line, match, RE_FLOAT)) // Have we reached numbers yet?
        {
          Tok tok(line, sep);
          Tok::iterator tokIt = tok.begin();
          unsigned int i;
          for(i = X; i <= Z && tokIt != tok.end(); ++i)
            mtx(vec, i) = boost::lexical_cast< double>(*tokIt++);

          // Did we get all three components?
          if(i <= Z)
            break;
          // Have we got all three lattice vectors
          if(++vec == 3)
            break;
        }
      }
      if(vec == 3)
        structure->setUnitCell(common::UnitCell(mtx.t()));
    }
  }

  // Reset the stream
  is.clear(); // Clear the EoF flag
  is.seekg(0, is.beg);

  const common::UnitCell * const unitCell = structure->getUnitCell();
  if(!unitCell)
    return common::types::StructurePtr();

  if(findNextLine(line, is, "%BLOCK positions_", false))
  {
    common::AtomSpeciesId::Value species;
    arma::vec3 pos;
    const bool fractional = boost::ifind_first(line, "_frac");
    const bool cartesian = boost::ifind_first(line, "_abs");
    if(fractional || cartesian)
    { // fractional format
      while(std::getline(is, line) && !::boost::ifind_first(line, "%ENDBLOCK"))
      {
        Tok tok(line, sep);
        Tok::iterator tokIt = tok.begin();
        species = *tokIt++;

        unsigned int i;
        for(i = X; i <= Z && tokIt != tok.end(); ++i)
          pos(i) = boost::lexical_cast< double>(*tokIt++);

        // Did we get all three components?
        if(i > Z)
        {
          if(fractional)
            unitCell->fracToCartInplace(pos);
          structure->newAtom(species).setPosition(pos);
        }
      }
    }
  }

  return structure;
}

void
CellReaderWriter::writeStructure(common::Structure & structure,
    const ResourceLocator & locator) const
{
  const fs::path filepath(locator.path());
  if(!filepath.has_filename())
    return; // Can't write out structure without filepath

  const fs::path dir(filepath.parent_path());
  if(!dir.empty() && !exists(dir))
    create_directories(dir);

  fs::ofstream strFile;
  strFile.open(filepath);

  writeStructure(strFile, structure);

  if(strFile.is_open())
    strFile.close();
}

void
CellReaderWriter::writePressureBlock(std::ostream & os,
    const arma::mat33 & pressureMtx) const
{
  os << "%BLOCK EXTERNAL_PRESSURE\n";
  for(int row = 0; row < 3; ++row)
  {
    os << std::string(row, ' ');
    for(int col = row; col < 3; ++col)
      os << pressureMtx(row, col) << " ";
    os << "\n";
  }
  os << "%ENDBLOCK EXTERNAL_PRESSURE\n";
}

void
CellReaderWriter::writeStructure(std::ostream & os,
    common::Structure & structure) const
{
  using namespace utility::cell_params_enum;

  // Use full precision when printing numbers
  os.precision(std::numeric_limits< double>::digits10);
  os << std::fixed;

  const common::UnitCell * unitCell = structure.getUnitCell();

  common::UnitCell boundingCell;
  if(!unitCell)
  {
    double latticeParams[6];
    os
        << "# WARNING: The following cell was created as a default because this structure didn't have one.\n";
    latticeParams[A] = 10;
    latticeParams[B] = 10;
    latticeParams[C] = 10;
    latticeParams[ALPHA] = 90.0;
    latticeParams[BETA] = 90.0;
    latticeParams[GAMMA] = 90.0;
    boundingCell.setLatticeParams(latticeParams);
    unitCell = &boundingCell;
  }

  writeLatticeBlock(os, *unitCell);
  os << "\n";
  writePositionsBlock(os, structure, *unitCell);
  os << "\n";
}

void
CellReaderWriter::writeLatticeBlock(std::ostream & os,
    const common::UnitCell & unitCell) const
{
  using namespace utility::cell_params_enum;

  const double (&params)[6] = unitCell.getLatticeParams();
  os << "%BLOCK LATTICE_ABC" << "\n";
  os << params[A] << " " << params[B] << " " << params[C] << "\n";
  os << params[ALPHA] << " " << params[BETA] << " " << params[GAMMA] << "\n";
  os << "%ENDBLOCK LATTICE_ABC" << std::endl;
}

void
CellReaderWriter::writePositionsBlock(std::ostream & os,
    const common::Structure & structure,
    const common::UnitCell & unitCell) const
{
  using namespace utility::cart_coords_enum;

  arma::mat positions;
  structure.getAtomPositions(positions);
  unitCell.cartsToFracInplace(positions);
  unitCell.wrapVecsFracInplace(positions);
  os << "%BLOCK POSITIONS_FRAC" << "\n";
  for(size_t i = 0; i < structure.getNumAtoms(); ++i)
  {
    const common::Atom & atom = structure.getAtom(i);
    os << atom.getSpecies() << " ";
    os << positions(X, i) << " " << positions(Y, i) << " " << positions(Z, i)
        << "\n";
  }
  os << "%ENDBLOCK POSITIONS_FRAC" << std::endl;
}

}
}
