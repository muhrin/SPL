/*
 * DistanceMatrixComparator.cpp
 *
 *  Created on: Aug 29, 2012
 *      Author: muhrin
 */

// INCLUDES /////////////////////////////////////
#include "spl/utility/DistanceMatrixComparator.h"

#include <iterator>

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

#include "spl/common/DistanceCalculator.h"
#include "spl/common/Structure.h"
#include "spl/math/NumberAlgorithms.h"
#include "spl/utility/GenericBufferedComparator.h"

// Turn on or off DistanceMatrixComparator (DMC) debugging
//#define SSLIB_DMC_DEBUG

namespace spl {
namespace utility {

typedef std::pair< size_t, double> IndexDoublePair;

const double DistanceMatrixComparator::STRUCTURES_INCOMPARABLE =
    std::numeric_limits< double>::max();
const double DistanceMatrixComparator::DEFAULT_TOLERANCE = 0.002;

bool
indexDoubleLessThan(const IndexDoublePair & p1, const IndexDoublePair & p2)
{
  return p1.second < p2.second;
}

DistanceMatrixComparisonData::DistanceMatrixComparisonData(
    const common::Structure & _structure)
{
  // Get a primitive setting version of the structure, otherwise the algorithm could get confused
  const UniquePtr< const common::Structure>::Type primitive(
      _structure.getPrimitiveCopy());

  const common::DistanceCalculator & distCalc =
      primitive->getDistanceCalculator();

  const size_t numAtoms = primitive->getNumAtoms();
  arma::mat unsortedDistnacesMatrix(numAtoms, numAtoms);
  unsortedDistnacesMatrix.diag().zeros();

  // Copy over the species of all the atoms (ordered by index)
  primitive->getAtomSpecies(std::back_inserter(speciesList));

  size_t i, j; // Loop indices used throughout
  const common::Atom * atomI;
  for(i = 0; i < numAtoms - 1; ++i)
  {
    atomI = &primitive->getAtom(i);
    for(size_t j = i + 1; j < numAtoms; ++j)
    {
      unsortedDistnacesMatrix(i, j) = distCalc.getDistMinImg(*atomI,
          primitive->getAtom(j));
    }
  }
  // Copy over symmetric elements to bottom left
  unsortedDistnacesMatrix = arma::symmatu(unsortedDistnacesMatrix);

  // Get the sum down each column
  arma::rowvec sums = arma::sum(unsortedDistnacesMatrix);

  // Get the set of species (i.e. each species only appearing once)
  std::set< common::AtomSpeciesId::Value> speciesSet(speciesList.begin(),
      speciesList.end());

  std::vector< size_t> indexRemapping(numAtoms);
  std::vector< IndexDoublePair> indexLengthList;
  size_t currentIdx = 0;
  BOOST_FOREACH(const common::AtomSpeciesId::Value & species, speciesSet)
  {
    indexLengthList.clear();
    for(i = 0; i < numAtoms; ++i)
    {
      if(speciesList[i] == species)
        indexLengthList.push_back(IndexDoublePair(i, sums(i)));
    }

    // Sort the species by total length of nearest neighbour distances (ascending)
    std::sort(indexLengthList.begin(), indexLengthList.end(),
        indexDoubleLessThan);

    for(j = 0; j < indexLengthList.size(); ++j)
      indexRemapping[indexLengthList[j].first] = currentIdx++;
  }

  // Now populate the distances matrix sorted by species and total nearest neighbour distances
  distancesMtx.set_size(numAtoms, numAtoms);
  for(i = 0; i < numAtoms; ++i)
  {
    for(j = 0; j < numAtoms; ++j)
    {
      distancesMtx(i, j) = unsortedDistnacesMatrix(indexRemapping[i],
          indexRemapping[j]);
    }
  }

}

DistanceMatrixComparator::DistanceMatrixComparator(
    const size_t fastComparisonAtomsLimit) :
    myFastComparisonAtomsLimit(fastComparisonAtomsLimit), myTolerance(
        DEFAULT_TOLERANCE)
{
}

DistanceMatrixComparator::DistanceMatrixComparator(const double tolerance,
    const size_t fastComparisonAtomsLimit) :
    myFastComparisonAtomsLimit(fastComparisonAtomsLimit), myTolerance(tolerance)
{
}

double
DistanceMatrixComparator::compareStructures(const common::Structure & str1,
    const common::Structure & str2) const
{
  ComparisonDataPtr comp1(generateComparisonData(str1));
  ComparisonDataPtr comp2(generateComparisonData(str2));
  return compareStructures(*comp1.get(), *comp2.get());
}

bool
DistanceMatrixComparator::areSimilar(const common::Structure & str1,
    const common::Structure & str2) const
{
  ComparisonDataPtr comp1(generateComparisonData(str1));
  ComparisonDataPtr comp2(generateComparisonData(str2));
  return areSimilar(*comp1.get(), *comp2.get());
}

boost::shared_ptr< IBufferedComparator>
DistanceMatrixComparator::generateBuffered() const
{
  return boost::shared_ptr< IBufferedComparator>(
      new GenericBufferedComparator< DistanceMatrixComparator>(*this));
}

std::auto_ptr< DistanceMatrixComparisonData>
DistanceMatrixComparator::generateComparisonData(
    const common::Structure & structure) const
{
  return std::auto_ptr< DistanceMatrixComparisonData>(
      new DistanceMatrixComparisonData(structure));
}

bool
DistanceMatrixComparator::areSimilar(const DataTyp & str1Data,
    const DataTyp & str2Data) const
{
  return compareStructures(str1Data, str2Data) < myTolerance;
}

double
DistanceMatrixComparator::compareStructures(const DataTyp & str1Data,
    const DataTyp & str2Data) const
{
  if(!areComparable(str1Data, str2Data))
    return STRUCTURES_INCOMPARABLE;

  const size_t maxAtoms = std::max(str1Data.distancesMtx.n_cols,
      str2Data.distancesMtx.n_cols);

  if(maxAtoms < myFastComparisonAtomsLimit)
  {
    // Use more expensive, but more accurate method
    return compareStructuresFull(str1Data, str2Data);
  }
  else
  {
    // Use cheaper method
    return compareStructuresFast(str1Data, str2Data);
  }
}

bool
DistanceMatrixComparator::areComparable(const DataTyp & str1Data,
    const DataTyp & str2Data) const
{
  return str1Data.distancesMtx.n_cols != 0 && str2Data.distancesMtx.n_cols != 0;
}

double
DistanceMatrixComparator::compareStructuresFull(const DataTyp & str1Data,
    const DataTyp & str2Data) const
{
  const size_t numAtoms1 = str1Data.distancesMtx.n_cols;
  const size_t numAtoms2 = str2Data.distancesMtx.n_cols;
  const unsigned int leastCommonMultiple = math::leastCommonMultiple(numAtoms1,
      numAtoms2);

  std::vector< size_t> indices(leastCommonMultiple);
  for(size_t i = 0; i < leastCommonMultiple; ++i)
    indices[i] = i;

  double r_ij1, r_ij2, distDiff;
  double sqSum = 0.0;
  double minSqSum = std::numeric_limits< double>::max();
  bool speciesMismatch;
  size_t i, j, iRem, permIRem;

  // Go through the permutations
  do
  {
    speciesMismatch = false;
    for(i = 0; i < leastCommonMultiple; ++i)
    {
      if(str1Data.speciesList[i % numAtoms1]
          != str2Data.speciesList[indices[i] % numAtoms2])
      {
        speciesMismatch = true;
        break;
      }
    }

    // If any of the species don't match up then skip
    if(!speciesMismatch)
    {
      sqSum = 0.0;
      for(i = 0; i < leastCommonMultiple; ++i)
      {
        iRem = i % numAtoms1;
        permIRem = indices[i] % numAtoms2;
        for(j = i + 1; j < leastCommonMultiple; ++j)
        {
          r_ij1 = str1Data.distancesMtx(iRem, j % numAtoms1);
          r_ij2 = str2Data.distancesMtx(permIRem, indices[j] % numAtoms2);

          if(r_ij1 + r_ij2 > 0)
          {
            distDiff = 2.0 * (r_ij1 - r_ij2) / (r_ij1 + r_ij2);
            sqSum += distDiff * distDiff;
            if(sqSum >= minSqSum)
              break;
          }
        }
        if(sqSum >= minSqSum)
          break;
      }

      minSqSum = std::min(sqSum, minSqSum);
    }
  } while(std::next_permutation(indices.begin(), indices.end()));

  return std::sqrt(minSqSum);
}

double
DistanceMatrixComparator::compareStructuresFast(const DataTyp & str1Data,
    const DataTyp & str2Data) const
{
  const size_t numAtoms1 = str1Data.distancesMtx.n_cols;
  const size_t numAtoms2 = str2Data.distancesMtx.n_cols;
  const unsigned int leastCommonMultiple = math::leastCommonMultiple(numAtoms1,
      numAtoms2);

  size_t i, j, iRem1, iRem2;
  double r_ij1, r_ij2, distDiff;
  double sqSum = 0.0;
  for(i = 0; i < leastCommonMultiple; ++i)
  {
    iRem1 = i % numAtoms1;
    iRem2 = i % numAtoms2;
    for(j = i + 1; j < leastCommonMultiple; ++j)
    {
      r_ij1 = str1Data.distancesMtx(iRem1, j % numAtoms1);
      r_ij2 = str2Data.distancesMtx(iRem2, j % numAtoms2);

      if(r_ij1 + r_ij2 > 0)
      {
        distDiff = 2.0 * (r_ij1 - r_ij2) / (r_ij1 + r_ij2);
        sqSum += distDiff * distDiff;
      }
    }
  }
  return sqrt(sqSum);
}

}
}
