/*
 * SpaceGroup.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#include "analysis/SpaceGroup.h"

#include <boost/algorithm/string.hpp>

extern "C"
{
#  include <spglib/spglib.h>
}

#include "common/Structure.h"

namespace sstbx {
namespace analysis {
namespace space_group {

//static const double DEFAULT_PRECISION = 0.05;

bool getSpacegroupInfo(
  SpacegroupInfo & outInfo,
  const common::Structure & structure,
  const double precision)
{
  if(!structure.getUnitCell())
    return false;

  double lattice[3][3];
  const common::UnitCell * const cell = structure.getUnitCell();
  const::arma::mat33 & orthoMtx = cell->getOrthoMtx();
  for(size_t i = 0; i < 3; ++i)
  {
    for(size_t j = 0; j < 3; ++j)
    {
      // Row-major = column-major
      lattice[i][j] = orthoMtx(i, j);
    }
  }

  const size_t numAtoms = structure.getNumAtoms();
  double (*positions)[3] = new double[numAtoms][3];
  ::arma::mat posMtx;
  structure.getAtomPositions(posMtx);
  cell->cartsToFracInplace(posMtx);
  for(size_t i = 0; i < numAtoms; ++i)
  {
    for(size_t j = 0; j < 3; ++j)
    {
      // Row-major = column-major
      positions[i][j] = posMtx(j, i);
    }
  }

  ::std::vector<common::AtomSpeciesId::Value> speciesVec;
  structure.getAtomSpecies(speciesVec);
  ::boost::scoped_array<int> species(new int[speciesVec.size()]);
  for(size_t i = 0; i < speciesVec.size(); ++i)
  {
    species[i] = speciesVec[i].ordinal();
  }
  
  // Get the space group
  SpglibDataset * spgData =
    spg_get_dataset(lattice, positions, species.get(), numAtoms, precision);

  // Extract the spacegroup info
  outInfo.number = (unsigned int)spgData->spacegroup_number;
  outInfo.iucSymbol = spgData->international_symbol;
  ::boost::algorithm::trim(outInfo.iucSymbol);
  outInfo.hallSymbol = spgData->hall_symbol;
  ::boost::algorithm::trim(outInfo.hallSymbol);

  // Clean up
  spg_free_dataset(spgData);
  delete [] positions;

  return true;
}

}
}
}
