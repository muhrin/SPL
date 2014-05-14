/*
 * PointSeparator.cpp
 *
 *  Created on: Nov 28, 2013
 *      Author: Martin Uhrin
 */

#include "spl/build_cell/PointSeparator.h"

#include <boost/foreach.hpp>

#include "spl/SSLibAssert.h"
#include "spl/common/AtomSpeciesDatabase.h"

//#define DEBUG_POINT_SEPARATOR

namespace spl {
namespace build_cell {

const int PointSeparator::DEFAULT_MAX_ITERATIONS = 1000;
const double PointSeparator::DEFAULT_TOLERANCE = 0.001;

SeparationData::SeparationData(const size_t numPoints,
    const common::DistanceCalculator & _distanceCalculator) :
    distanceCalculator(_distanceCalculator)
{
  init(numPoints);
}

SeparationData::SeparationData(const common::Structure & structure) :
    distanceCalculator(structure.getDistanceCalculator())
{
  const size_t numPoints = structure.getNumAtoms();
  init(numPoints);
  structure.getAtomPositions(points);
}

SeparationData::SeparationData(const common::Structure & structure,
    const common::AtomSpeciesDatabase & db) :
    distanceCalculator(structure.getDistanceCalculator())
{
  typedef std::vector< common::AtomSpeciesId::Value> Labels;
  typedef std::set< common::AtomSpeciesId::Value> Species;
  typedef std::map< SpeciesPair, double> SepList;

  const size_t numPoints = structure.getNumAtoms();
  init(numPoints);
  structure.getAtomPositions(points);

  // Get the list of atom species in order
  Labels labels;
  structure.getAtomSpecies(std::back_inserter(labels));

  Species species(labels.begin(), labels.end());
  SepList sepList;

  OptionalDouble dist;
  for(Species::const_iterator i = species.begin(); i != species.end(); ++i)
  {
    for(Species::const_iterator j = i; j != species.end(); ++j)
    {
      const SpeciesPair pair(*i, *j);
      dist = db.getSpeciesPairDistance(pair);
      if(dist)
        sepList[pair] = *dist;
    }
  }

  setSeparationsFromLabels(labels, sepList);
}

void
SeparationData::init(const size_t numPoints)
{
  points.zeros(3, numPoints);
  separations.zeros(numPoints, numPoints);
}

PointSeparator::PointSeparator() :
    myMaxIterations(DEFAULT_MAX_ITERATIONS), myTolerance(DEFAULT_TOLERANCE)
{
}

PointSeparator::PointSeparator(const size_t maxIterations,
    const double tolerance) :
    myMaxIterations(maxIterations), myTolerance(tolerance)
{
}

bool
PointSeparator::separatePoints(SeparationData * const sepData) const
{
  using std::sqrt;

  SSLIB_ASSERT(sepData);

  const FixedList & fixed = generateFixedList(*sepData);
  const arma::mat minSepSqs = sepData->separations % sepData->separations;

  const size_t numPoints = sepData->points.n_cols;
  if(numPoints == 0)
    return true;

  double sep, sepSq, sepDiff;
  double maxOverlapFraction;
  double prefactor; // Used to adjust the displacement vector if either atom is fixed
  arma::vec3 dr, sepVec;
  arma::mat delta(3, numPoints);
  bool success = false;

  for(size_t iters = 0; iters < myMaxIterations; ++iters)
  {
    // First loop over calculating separations and checking for overlap
    maxOverlapFraction = calcMaxOverlapFraction(*sepData, minSepSqs, fixed);

#ifdef DEBUG_POINT_SEPARATOR
    std::cout << sepData->points.t() << "\n\n";
#endif

    if(maxOverlapFraction < myTolerance)
    {
      success = true;
      break;
    }

    delta.zeros();
    // Now fix-up any overlaps
    for(size_t row = 0; row < numPoints - 1; ++row)
    {
      for(size_t col = row + 1; col < numPoints; ++col)
      {
        if(!(fixed[row] && fixed[col]))
        {
          sepVec = sepData->distanceCalculator.getVecMinImg(
              sepData->points.col(row), sepData->points.col(col));
          sepSq = arma::dot(sepVec, sepVec);
          if(sepSq < minSepSqs(row, col))
          {
            // If both are free then share the displacement, otherwise all goes to one
            prefactor = fixed[row] || fixed[col] ? 1.0 :  0.5;

            if(sepSq != 0.0)
            {
              sep = sqrt(sepSq);
              sepDiff = sepData->separations(row, col) - sep;

              // Generate the displacement vector
              dr = prefactor * sepDiff / sep * sepVec;
            }
            else // overlapping, so perturb randomly
            {
              dr = arma::randu(3);
              dr *= 0.001 * sepData->separations(row, col) / arma::dot(dr, dr);
            }
            // Move them
            if(!fixed[row])
              delta.col(row) -= dr;
            if(!fixed[col])
              delta.col(col) += dr;
          }
        }
      }
    }
    sepData->points += delta;
  }

  return success;
}

PointSeparator::FixedList
PointSeparator::generateFixedList(const SeparationData & sepData) const
{
  FixedList fixed(sepData.points.n_cols, false);
  BOOST_FOREACH(const size_t idx, sepData.fixedPoints)
    fixed[idx] = true;
  return fixed;
}

double
PointSeparator::calcMaxOverlapFraction(const SeparationData & sepData,
    const arma::mat & minSepSqs, const FixedList & fixed) const
{
  const size_t numPoints = sepData.points.n_cols;

  double sepSq, maxOverlapSq = 0.0;
  for(size_t row = 0; row < numPoints - 1; ++row)
  {
    const arma::vec3 & posI = sepData.points.col(row);
    for(size_t col = row + 1; col < numPoints; ++col)
    {
      if(!(fixed[row] && fixed[col])) // Only if they're not both fixed
      {
        const arma::vec3 & posJ = sepData.points.col(col);

        sepSq = sepData.distanceCalculator.getDistSqMinImg(posI, posJ);

        if(sepSq < minSepSqs(row, col))
          maxOverlapSq = std::max(maxOverlapSq, minSepSqs(row, col) / sepSq);
      }
    }
  }
  // TODO: CHECK THIS
  if(maxOverlapSq == 0.0)
    return 0.0;
  else
    return 1.0 - 1.0 / std::sqrt(maxOverlapSq);
}

}
}
