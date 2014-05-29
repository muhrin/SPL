/*
 * Utility.cpp
 *
 *  Created on: May 22, 2014
 *      Author: Martin Uhrin
 */

// INCLUEDES /////////////
#include "spl/build_cell/Utility.h"

#include <boost/foreach.hpp>

#include "spl/build_cell/Shapes.h"

namespace spl {
namespace build_cell {

double
clusterVolume(const common::AtomsFormula & atoms,
    const common::AtomSpeciesDatabase & speciesDb)
{
  typedef common::AtomsFormula::const_iterator FormulaIterator;

  if(atoms.isEmpty())
    return 0.0;

  double vol = 0.0;
  for(FormulaIterator itI = atoms.begin(), end = atoms.end(); itI != end; ++itI)
  {
    for(FormulaIterator itJ = itI; itJ != end; ++itJ)
    {
      const boost::optional< double> & d_ij =
          speciesDb.getSpeciesPairDistance(SpeciesPair(itI->first, itJ->first));
      if(d_ij)
      {
        // Number of pairs between species A and B is:
        // N_A * (N_A - 1) / 2 iff A == B
        // N_A * N_B otherwise
        const int numPairs =
            itI == itJ ?
                itI->second * (itI->second - 1) / 2 : itI->second * itJ->second;
        vol += static_cast< double>(numPairs) * sphere::volume(*d_ij * 0.5);
      }
    }
  }

  return 2.0 * vol / static_cast< double>(atoms.total() - 1);
}

double
naturalVolume(const common::AtomsFormula & atoms,
    const common::AtomSpeciesDatabase & speciesDb, const bool periodic)
{
  common::AtomsFormula composition = atoms;
  if(periodic)
    composition *= 27; // Create 3x3x3 supercell

  const double vol = clusterVolume(composition, speciesDb);

  return periodic ? vol / 27.0 : vol;
}

}
}
