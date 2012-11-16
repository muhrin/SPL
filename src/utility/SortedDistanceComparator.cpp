/*
 * SortedDistanceComparator.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: muhrin
 */

// INCLUDES /////////////////////////////////////

#include "utility/SortedDistanceComparator.h"

#include <memory>

#include <boost/scoped_ptr.hpp>

#include <armadillo>

#include "common/DistanceCalculator.h"
#include "common/Structure.h"
#include "common/UnitCell.h"
#include "utility/GenericBufferedComparator.h"
#include "utility/Math.h"

#define SORTED_DIST_COMP_DEBUG (SSLIB_DEBUG & 0)

namespace sstbx {
namespace utility {

const size_t SortedDistanceComparator::MAX_CELL_MULTIPLES   = 10;
const double SortedDistanceComparator::DEFAULT_TOLERANCE    = 1e-4;
const double SortedDistanceComparator::CUTOFF_FACTOR        = 1.0;


SortedDistanceComparisonData::SortedDistanceComparisonData(const common::Structure & structure, const bool usePrimitive)
{
  const common::Structure * primitive = &structure;

  // This needs to be in this scope so it lasts until we return
  common::StructurePtr primitiveCopy;
  if(usePrimitive)
  {
    primitiveCopy = structure.getPrimitiveCopy();
    primitive = primitiveCopy.get();
  }

  const common::UnitCell * const unitCell = primitive->getUnitCell();
  if(unitCell)
  {
    ::arma::vec3 diag = unitCell->getLongestDiagonal();
    const double longestDiag = sqrt(::arma::dot(diag, diag));
    cutoff = SortedDistanceComparator::CUTOFF_FACTOR * longestDiag;
  }

  const common::DistanceCalculator & distCalc = primitive->getDistanceCalculator();

  // Get the unit cell and number of atoms, need to do this as making the
  // structure primitive may have changed these so we need to store them
  volume = unitCell->getVolume();
  numAtoms = primitive->getNumAtoms();

  primitive->getAtomSpecies(species);
  ::std::set<common::AtomSpeciesId::Value> speciesSet(species.begin(), species.end());
  species.resize(speciesSet.size());
  ::std::copy(speciesSet.begin(), speciesSet.end(), species.begin());
  const size_t numSpecies = species.size();

  initSpeciesDistancesMap();

	// Calculate the distances ...
  common::AtomSpeciesId::Value specI, specJ;
  DistancesVecPtr distVecIJ;
  for(size_t i = 0; i < numAtoms; ++i)
  {
    const common::Atom & atomI = primitive->getAtom(i);
    specI = atomI.getSpecies();
    DistancesMap & iDistMap = speciesDistancesMap[specI];

    // Now to all the others
    for(size_t j = 0; j < numAtoms; ++j)
    {
      const common::Atom & atomJ = primitive->getAtom(j);
      specJ = atomJ.getSpecies();
      distVecIJ = iDistMap[specJ];

      distCalc.getDistsBetween(atomI, atomJ, cutoff, *distVecIJ);
    }
  }

	// ... and sort them
  for(size_t i = 0; i < numSpecies; ++i)
  {
    specI = species[i];
    DistancesMap & iDistMap = speciesDistancesMap[specI];
    for(size_t j = i; j < numSpecies; ++j)
    {
      specJ = species[j];
      distVecIJ = iDistMap[specJ];
      ::std::sort(distVecIJ->begin(), distVecIJ->end());
    }
  }
}

void SortedDistanceComparisonData::initSpeciesDistancesMap()
{
  const size_t numSpecies = species.size();
  common::AtomSpeciesId::Value specI, specJ;
  for(size_t i = 0; i < numSpecies; ++i)
  {
    specI = species[i];
    DistancesMap & distMap = speciesDistancesMap[specI];
    for(size_t j = i; j < numSpecies; ++j)
    {
      specJ = species[j];
      DistancesVecPtr & distVec = distMap[specJ];
      distVec.reset(new ::std::vector<double>());
      speciesDistancesMap[specJ][specI] = distVec;
    }
  }
}

SortedDistanceComparator::SortedDistanceComparator(const double tolerance, const bool volumeAgnostic, const bool usePrimitive):
myScaleVolumes(volumeAgnostic),
myUsePrimitive(usePrimitive),
myTolerance(tolerance)
{}

double SortedDistanceComparator::compareStructures(
	const sstbx::common::Structure & str1,
	const sstbx::common::Structure & str2) const
{
  ::std::auto_ptr<const SortedDistanceComparator::DataTyp> comp1(generateComparisonData(str1));
  ::std::auto_ptr<const SortedDistanceComparator::DataTyp> comp2(generateComparisonData(str2));

  return compareStructures(str1, *comp1, str2, *comp2);
}

bool SortedDistanceComparator::areSimilar(
	const sstbx::common::Structure & str1,
	const sstbx::common::Structure & str2) const
{
  ::std::auto_ptr<const SortedDistanceComparator::DataTyp> comp1(generateComparisonData(str1));
  ::std::auto_ptr<const SortedDistanceComparator::DataTyp> comp2(generateComparisonData(str2));

  return areSimilar(str1, *comp1, str2, *comp2);
}

double SortedDistanceComparator::compareStructures(
    const common::Structure & str1,
		const SortedDistanceComparisonData & dist1,
    const common::Structure & str2,
		const SortedDistanceComparisonData & dist2) const
{
  typedef ::std::vector<double> DistancesVec;
  typedef StridedIndexAdapter<size_t> IndexAdapter;

  const size_t numSpecies = dist1.species.size();

  if(numSpecies != dist2.species.size())
  {
    // Species mismatch!
    return ::std::numeric_limits<double>::max();
  }

  // Set up the adapters
  const unsigned int leastCommonMultiple = math::leastCommonMultiple(dist1.numAtoms, dist2.numAtoms);
  IndexAdapter adapt1(leastCommonMultiple / dist1.numAtoms);
  IndexAdapter adapt2(leastCommonMultiple / dist2.numAtoms);

  double scaleFactor = 1.0;
  if(myScaleVolumes)
  {
    // v2 / n2 * v1 / n1
    scaleFactor = (dist2.volume * dist1.numAtoms) / (dist1.volume * dist2.numAtoms);
  }
  else
  {
    scaleFactor = (double)dist2.numAtoms / (double)dist1.numAtoms;
  }

  double max = ::std::numeric_limits<double>::min();
  double sqSum = 0.0;
  unsigned int totalCompared = 0;
  common::AtomSpeciesId::Value specI, specJ;
  for(size_t i = 0; i < numSpecies - 1; ++i)
  {
    specI = dist1.species[i];
    
    // Do others
    const SortedDistanceComparisonData::DistancesMap & distMapI1 = dist1.speciesDistancesMap(specI);
    const SortedDistanceComparisonData::DistancesMap & distMapI2 = dist2.speciesDistancesMap(specI);
    for(size_t j = i; j < numSpecies; ++j)
    {
      specJ = dist1.species[j];
      calcProperties(*distMapI1(specJ), adapt1, *distMapI2(specJ), adapt2, sqSum, max, totalCompared, scaleFactor);
    }
  }

  return sqrt(sqSum / totalCompared);
  //return max;
}

bool SortedDistanceComparator::areSimilar(
    const common::Structure & str1,
		const SortedDistanceComparisonData & dist1,
    const common::Structure & str2,
		const SortedDistanceComparisonData & dist2) const
{
	return compareStructures(str1, dist1, str2, dist2) < myTolerance;
}

::std::auto_ptr<SortedDistanceComparisonData>
SortedDistanceComparator::generateComparisonData(const sstbx::common::Structure & str) const
{
	return ::std::auto_ptr<SortedDistanceComparisonData>(new SortedDistanceComparisonData(str, myUsePrimitive));
}

::boost::shared_ptr<SortedDistanceComparator::BufferedTyp> SortedDistanceComparator::generateBuffered() const
{
  return ::boost::shared_ptr<IBufferedComparator>(
    new GenericBufferedComparator<SortedDistanceComparator>(*this)
  );
}

void SortedDistanceComparator::calcProperties(
  const SortedDistanceComparator::DistancesVec & dist1,
  const StridedIndexAdapter<size_t> & adapt1,
  const SortedDistanceComparator::DistancesVec & dist2,
  const StridedIndexAdapter<size_t> & adapt2,
  double & sqSum,
  double & max,
  unsigned int & runningComparedTotal,
  const double scaleFactor) const
{
  const size_t maxIdx = ::std::min(adapt1.inv(dist1.size()), adapt2.inv(dist2.size()));

  double d1, d2, sum, delta;
  for(size_t i = 0; i < maxIdx; ++i)
  {
    d1 = dist1[adapt1(i)];
    d2 = scaleFactor * dist2[adapt2(i)];

#if SORTED_DIST_COMP_DEBUG
    if(std::abs(d1 - d2) > 1e-5)
      std::cout << "Diff is: " << std::abs(d1 - d2) << std::endl;
#endif
    sum = d1 + d2;
    delta = sum > 0 ? std::abs(d1 - d2) / sum : 0;
    max   = std::max(max, delta);
    sqSum   += delta * delta;
  }
  runningComparedTotal += maxIdx;
}

}
}
