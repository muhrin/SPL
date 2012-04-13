/*
 * ResReaderWriter.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: Martin Uhrin
 */

// INCLUDES //////////////////////////////////
#include "ResReaderWriter.h"

#include "AdditionalData.h"
#include "../common/AbstractFmidCell.h"
#include "../common/Atom.h"
#include "../common/AtomSpeciesDatabase.h"
#include "../common/AtomSpeciesId.h"
#include "../common/AtomSpeciesInfo.h"
#include "../common/Structure.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>

#include <armadillo>

#include <iomanip>
#include <set>
#include <vector>


// NAMESPACES ////////////////////////////////


namespace sstbx { namespace io {

void ResReaderWriter::writeStructure(
	const sstbx::common::Structure &str,
	const::boost::filesystem::path & filepath,
	const AdditionalData *const data) const
{
	writeStructure(str, filepath, ::sstbx::common::AtomSpeciesDatabase::inst(), data);
}

void ResReaderWriter::writeStructure(
	const ::sstbx::common::Structure & str,
	const ::boost::filesystem::path & filepath,
	const ::sstbx::common::AtomSpeciesDatabase & speciesDb,
	const AdditionalData * const data) const
{
	using namespace ::boost::filesystem;
	using ::sstbx::common::AtomSpeciesId;
	using ::std::endl;
	using ::boost::filesystem::ofstream;

	if(!filepath.has_filename())
		throw "Cannot write out structure without filepath";

	const path dir = filepath.parent_path();
	if(!dir.empty() && !exists(dir))
	{
		create_directory(dir);
	}

	ofstream strFile;
	strFile.open(filepath);

	//////////////////////////
	// Start title
	strFile << "TITL ";
	
	if(data && data->name)
		strFile << *data->name;
	else
		strFile << filepath.stem();
	
	// Presssure
	strFile << " ";
	if(data && data->pressure)
		strFile << *data->pressure;
	else
		strFile << "n/a";

	// Volume
	strFile << " " << str.getUnitCell()->getVolume();

	// Enthalpy
	strFile << " ";
	if(data && data->enthalpy)
		strFile << *data->enthalpy;
	else
		strFile << "n/a";

	// Space group
	strFile << " 0 0 (";
	if(data && data->spaceGroup)
		strFile << *data->spaceGroup;
	else
		strFile << "n/a";

	// Times found
	strFile << ") n - ";
	if(data && data->timesFound)
		strFile << *data->timesFound;
	else
		strFile << "n/a";

	strFile << endl;
	// End title //////////////////
	
	///////////////////////////////////
	// Start lattice
	const double (&latticeParams)[6] = str.getUnitCell()->getLatticeParams();

	// Do cell parameters
	strFile << "CELL 1";
	for(size_t i = 0; i < 6; ++i)
	{
		strFile << " " << latticeParams[i];
	}
	strFile << endl;

	strFile << "LATT -1" << endl;

	// End lattice

	////////////////////////////
	// Start atoms

	// Get the species and positions of all atoms
	using std::vector;
	using std::set;

	arma::Mat<double> positions;
	str.getAtomPositionsDescendent(positions);

	vector<AtomSpeciesId> species;
	str.getAtomSpeciesDescendent(species);

	set<AtomSpeciesId> uniqueSpecies(species.begin(), species.end());

	// Output atom species
	::std::map<AtomSpeciesId, ::std::string> speciesSymbols;
	strFile << "SFAC";
	BOOST_FOREACH(AtomSpeciesId id, uniqueSpecies)
	{
		const ::std::string * const name = speciesDb.getSymbol(id);
		speciesSymbols[id] = name ? *name : "?";
		strFile << " " << speciesSymbols[id];
	}

	// Now write out the atom positions along with the spcies
	const sstbx::common::AbstractFmidCell<double> * cell = str.getUnitCell();
	sstbx::common::Atom::Vec3 fracPos;
	for(size_t i = 0; i < positions.n_cols; ++i)
	{
		fracPos = cell->fractionalise(positions.col(i));
		strFile << endl << ::std::setprecision(32) << speciesSymbols[species[i]] << " " << 1 << " " <<
			fracPos[0] << " " << fracPos[1] << " " << fracPos[2] << " 1.0";
	}

	// End atoms ///////////

	strFile << endl << "END" << endl;


	strFile.close();
}

::std::vector<std::string> ResReaderWriter::getSupportedFileExtensions() const
{
	::std::vector<std::string> ext;
	ext.push_back("res");
	return ext;
}

}}