/*
 * ResReaderWriter.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "io/ResReaderWriter.h"

#include <iomanip>
#include <set>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include <armadillo>

#include "common/Atom.h"
#include "common/AtomSpeciesDatabase.h"
#include "common/AtomSpeciesId.h"
#include "common/AtomSpeciesInfo.h"
#include "common/Structure.h"
#include "common/StructureProperties.h"
#include "common/Types.h"
#include "common/UnitCell.h"
#include "io/IoFunctions.h"
#include "io/BoostFilesystem.h"
#include "utility/IndexingEnums.h"

// DEFINES /////////////////////////////////


// NAMESPACES ////////////////////////////////


namespace sstbx {
namespace io {

namespace fs = ::boost::filesystem;
namespace ssc = ::sstbx::common;
namespace properties = ssc::structure_properties;

const unsigned int ResReaderWriter::DIGITS_AFTER_DECIMAL = 8;

void ResReaderWriter::writeStructure(
	::sstbx::common::Structure & str,
	const ::boost::filesystem::path & filepath,
	const ::sstbx::common::AtomSpeciesDatabase & speciesDb) const
{
  using namespace utility::cell_params_enum;
	using ::sstbx::common::AtomSpeciesId;
	using ::std::endl;

  const double * dValue;
  const ::std::string * sValue;
  const unsigned int * uiValue;

	if(!filepath.has_filename())
		throw "Cannot write out structure without filepath";

  const fs::path dir = filepath.parent_path();
	if(!dir.empty() && !exists(dir))
	{
		create_directories(dir);
	}

  fs::ofstream strFile;
	strFile.open(filepath);

  const common::UnitCell * const cell = str.getUnitCell();

	//////////////////////////
	// Start title
	strFile << "TITL ";
	
	if(!str.getName().empty())
		strFile << str.getName();
	else
		strFile << filepath.stem();
	
	// Presssure
  strFile << " ";
  dValue = str.getProperty(properties::general::PRESSURE_INTERNAL);
	if(dValue)
    io::writeToStream(strFile, *dValue, DIGITS_AFTER_DECIMAL);
	else
		strFile << "n/a";

	// Volume
	strFile << " ";
  if(cell)
    strFile << cell->getVolume();
  else
    strFile << "n/a";

	// Enthalpy
	strFile << " ";
  dValue = str.getProperty(properties::general::ENERGY_INTERNAL);
	if(dValue)
		io::writeToStream(strFile, *dValue, DIGITS_AFTER_DECIMAL);
	else
		strFile << "n/a";

	// Space group
	strFile << " 0 0 (";
  sValue = str.getProperty(properties::general::SPACEGROUP_SYMBOL);
	if(sValue)
		strFile << *sValue;
	else
		strFile << "n/a";

	// Times found
	strFile << ") n - ";
  uiValue = str.getProperty(properties::searching::TIMES_FOUND);
	if(uiValue)
		strFile << *uiValue;
	else
		strFile << "n/a";

	strFile << endl;
	// End title //////////////////
	
	///////////////////////////////////
	// Start lattice
  if(cell)
  {
	  const double (&latticeParams)[6] = cell->getLatticeParams();

	  // Do cell parameters
	  strFile << "CELL 1.0";
	  for(size_t i = A; i <= GAMMA; ++i)
	  {
		  strFile << " " << latticeParams[i];
	  }
	  strFile << endl;
  }
	strFile << "LATT -1" << endl;

	// End lattice

	////////////////////////////
	// Start atoms

	// Get the species and positions of all atoms
	using std::vector;
	using std::set;

  ::arma::mat positions;
	str.getAtomPositions(positions);

  vector<AtomSpeciesId::Value> species;
	str.getAtomSpecies(species);

  set<AtomSpeciesId::Value> uniqueSpecies(species.begin(), species.end());

	// Output atom species
  std::map<AtomSpeciesId::Value, std::string> speciesSymbols;
  std::map<AtomSpeciesId::Value, unsigned int> speciesOrder;
	strFile << "SFAC";
  size_t idx = 1;
  BOOST_FOREACH(const AtomSpeciesId::Value id, uniqueSpecies)
	{
		const ::std::string * const name = speciesDb.getSymbol(id);
		speciesSymbols[id] = name ? *name : "?";
    speciesOrder[id]   = idx;
    ++idx;
		strFile << " " << speciesSymbols[id];
	}

	// Now write out the atom positions along with the spcies
	::arma::vec3 fracPos;
	for(size_t i = 0; i < positions.n_cols; ++i)
	{
    const AtomSpeciesId::Value id = species[i];
    if(cell)
		  fracPos = cell->cartToFrac(positions.col(i));
    else
      fracPos = positions.col(i);

		strFile << endl << speciesSymbols[id] << " " << speciesOrder[id] << " " <<
			::std::setprecision(12) << fracPos[0] << " " << fracPos[1] << " " << fracPos[2] << " 1.0";
	}

	// End atoms ///////////

	strFile << endl << "END" << endl;

  str.setProperty(
    properties::io::LAST_ABS_FILE_PATH,
    io::absolute(filepath));

 if(strFile.is_open())
    strFile.close();
}

UniquePtr<common::Structure>::Type ResReaderWriter::readStructure(
	const boost::filesystem::path &     filepath,
	const sstbx::common::AtomSpeciesDatabase & speciesDb) const
{
  namespace utility = ::sstbx::utility;
  using sstbx::common::Atom;
	using sstbx::common::AtomSpeciesId;
	using std::endl;
  using std::getline;
  using boost::bad_lexical_cast;
  using boost::lexical_cast;
	using boost::filesystem::ifstream;

	if(!filepath.has_filename())
		throw "Cannot write out structure without filepath";

  UniquePtr<common::Structure>::Type str;

  if(!exists(filepath))
    return str;

	ifstream strFile;
	strFile.open(filepath);

  // Set up our tokenizer to split around space and tab
	typedef boost::tokenizer<boost::char_separator<char> > Tok;
	const boost::char_separator<char> sep(" \t");

  bool foundTitle, foundCell, foundSfac;
  if(strFile.is_open())
  {
    bool fileReadSuccessfully = true;
    str.reset(new common::Structure());

    str->setProperty(
      properties::io::LAST_ABS_FILE_PATH,
      io::absolute(filepath));

    std::string line;

    // We're expecting the TITL line
    foundTitle = false;
    for(
      getline(strFile, line);
      !foundTitle && strFile.good();
      getline(strFile, line))
      {
      Tok toker(line, sep);
      Tok::iterator tokIt = toker.begin();
      if(*tokIt == "TITL")
      {
        foundTitle = true;
        if(++tokIt != toker.end())
          str->setName(*tokIt);
        else
          str->setName(io::stemString(filepath));

        bool hasMore = true;
        // Parse the rest of the tokens
        // Pressure
        if(hasMore && ++tokIt != toker.end())
        {
          try
          {
            str->setPropertyFromString(properties::general::PRESSURE_INTERNAL, *tokIt);
          }
          catch(const bad_lexical_cast &)
          {}
        }
        else
          hasMore = false;

        // Volume
        if(hasMore && ++tokIt == toker.end())
          hasMore = false;

        // Free energy
        if(hasMore && ++tokIt != toker.end())
        {
          try
          {
            str->setPropertyFromString(properties::general::ENERGY_INTERNAL, *tokIt);
          }
          catch(const bad_lexical_cast &)
          {}
        }
        else 
          hasMore = false;

        if(hasMore && ++tokIt == toker.end())
          hasMore = false;
        if(hasMore && ++tokIt == toker.end())
          hasMore = false;

        // Space group
        if(hasMore && ++tokIt != toker.end())
        {
          ::std::string iucSymbol = *tokIt;
          if(!iucSymbol.empty() && iucSymbol[0] == '(')
            iucSymbol.erase(0, 1);
          if(!iucSymbol.empty() && iucSymbol[iucSymbol.size() - 1] == ')')
            iucSymbol.erase(iucSymbol.size() - 1, 1);

          str->setProperty(properties::general::SPACEGROUP_SYMBOL, iucSymbol);
        }
        else
          hasMore = false;

        if(hasMore && ++tokIt == toker.end())
          hasMore = false;
        if(hasMore && ++tokIt == toker.end())
          hasMore = false;

        // Times found
        if(hasMore && ++tokIt != toker.end())
        {
          try
          {
            str->setPropertyFromString(properties::searching::TIMES_FOUND, *tokIt);
          }
          catch(const bad_lexical_cast &)
          {}
        }
        else
          hasMore = false;
      } // end if(*tokIt == "TITL")
    } // end for

    // We're expecting the CELL line
    foundCell = false;
    for(; // The previous for statement will have called one last getline
      !foundCell && strFile.good();
      getline(strFile, line))
    {
      Tok toker(line, sep);
      Tok::iterator tokIt = toker.begin();
      if(*tokIt == "CELL")
      {
        foundCell = true;

        // Move the token on
        bool hasMore = ++tokIt != toker.end();

        if(hasMore)
        {
          // Set up the cell parameters
          double params[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

          unsigned int i = 0;
          bool foundParams = true;
          for(++tokIt; i < 6 && tokIt != toker.end();
            ++i, ++tokIt)
          {
            try
            {
              params[i] = lexical_cast<double>(*tokIt);
            }
            catch(const bad_lexical_cast &)
            {
              foundParams = false;
              break;
            }
          }
          // Check if we found all six values
          foundParams = foundParams && i == 6;

          str->setUnitCell(common::UnitCellPtr(new common::UnitCell(params)));
        } // end if(hasMore)
      } // end if(*tokIt == "CELL")
    } // while !foundCell


    // Look for SFAC line
    foundSfac = false;
    for(; // The previous for statement will have called one last getline
      !foundSfac && strFile.good();
      getline(strFile, line))
    {
      Tok toker(line, sep);
      Tok::iterator tokIt = toker.begin();
      if(*tokIt == "SFAC")
      {
        foundSfac = true;

        // Skip over the first line, it just outlines overall species
        getline(strFile, line);

        // Now loop over all atoms
        bool foundEnd = false;
        for(; // The previous for statement will have called one last getline
          !foundEnd && strFile.good();
          getline(strFile, line))
        {
          bool atomFound = true;      

          Tok atomToker(line, sep);
          Tok::iterator atomTokIt = atomToker.begin();

          if(atomTokIt == atomToker.end() || *atomTokIt == "END")
          {
            foundEnd = true;
            break;
          }

          // Try finding the species id
          const AtomSpeciesId::Value id = speciesDb.getIdFromSymbol(*atomTokIt);

          if(id != sstbx::common::AtomSpeciesId::DUMMY)
          {
            bool hasMore = true;

            // Skip over first value
            hasMore = (++atomTokIt != atomToker.end());

            if(hasMore)
            {
              // Try to get the coordinates
              bool readCoordinates = true;
              arma::vec3 pos;
              unsigned int coord = 0;
              for(++atomTokIt;
                coord < 3 && atomTokIt != atomToker.end();
                ++coord, ++atomTokIt)
              {
                try
                {
                  pos(coord) = lexical_cast<double>(*atomTokIt);
                }
                catch(const bad_lexical_cast &)
                {
                  readCoordinates = false;
                  break;
                }
              }
              readCoordinates = readCoordinates && coord == 3;

              if(readCoordinates)
              {
                const common::UnitCell * const cell = str->getUnitCell();
                Atom & atom = str->newAtom(id);
                // Try to orthoginalise the position
                if(cell)
                {
                  cell->fracToCartInplace(pos);
                }
                atom.setPosition(pos);
              }
            } // end if(hasMore)
          }

        } // for all atoms

      } // end if(*tokIt == "SFAC")
    } // while !foundSfac
  
    strFile.close();
  } // end if(strFile.is_open())


  if(!foundTitle || !foundCell || !foundSfac)
    str.reset();

  return str;
}

std::vector<std::string> ResReaderWriter::getSupportedFileExtensions() const
{
	std::vector<std::string> ext;
	ext.push_back("res");
	return ext;
}

}}
