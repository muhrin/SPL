/*
 * PointSeparator.h
 *
 *  Created on: Nov 28, 2013
 *      Author: Martin Uhrin
 */

#ifndef POINT_SEPARATOR_H
#define POINT_SEPARATOR_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#include <map>
#include <set>
#include <vector>

#include <armadillo>

#include "spl/common/AtomSpeciesId.h"
#include "spl/utility/Range.h"

namespace spl {
namespace common {
class Atom;
class AtomSpeciesDatabase;
class DistanceCalculator;
class Structure;
}

namespace build_cell {

struct SeparationData
{
  typedef ::arma::mat Points;
  typedef ::std::set<size_t> FixedPoints;

  SeparationData(const size_t numPoints,
      const common::DistanceCalculator & distanceCalculator);
  SeparationData(const common::Structure & structure);
  SeparationData(const common::Structure & structure,
      const common::AtomSpeciesDatabase & db);

  template <typename Label>
  void
  setSeparationsFromLabels(const ::std::vector<Label> & pointLabels,
      ::std::map< utility::OrderedPair<Label>, double> & sepList);

  const common::DistanceCalculator & distanceCalculator;
  Points points;
  ::arma::mat separations;
  FixedPoints fixedPoints;
private:
  void
  init(const size_t numPoints);
};

class PointSeparator
{
public:
  static const int DEFAULT_MAX_ITERATIONS;
  static const double DEFAULT_TOLERANCE;

  PointSeparator();
  PointSeparator(const size_t maxIterations, const double tolerance);

  bool
  separatePoints(SeparationData * const sepData) const;

private:
  typedef ::std::vector< bool> FixedList;

  FixedList
  generateFixedList(const SeparationData & sepData) const;
  double
  calcMaxOverlapFraction(const SeparationData & sepData,
      const ::arma::mat & minSepSqs, const FixedList & fixed) const;

  const size_t myMaxIterations;
  const double myTolerance;
};


}
}

#include "spl/build_cell/detail/PointSeparator.h"

#endif /* POINT_SEPARATOR_H */
