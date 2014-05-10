/*
 * SortedDistanceComparator.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef SORTED_DISTANCE_COMPARATOR_H
#define SORTED_DISTANCE_COMPARATOR_H

// INCLUDES /////////////////////////////////////////////
#include "spl/SSLib.h"

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include "spl/common/AtomSpeciesId.h"
#include "spl/utility/IStructureComparator.h"
#include "spl/utility/IndexAdapters.h"
#include "spl/utility/MapEx.h"

namespace spl {
// FORWARD DECLARATIONS ////////////////////////////////////
namespace common {
class Structure;
}
namespace math {
class RunningStats;
}

namespace utility {

class SortedDistanceComparisonData
{
public:
  typedef ::std::vector< double> DistancesVec;
  typedef ::boost::shared_ptr< DistancesVec> DistancesVecPtr;

  typedef utility::MapEx< common::AtomSpeciesId::Value, DistancesVecPtr> DistancesMap;
  typedef utility::MapEx< common::AtomSpeciesId::Value, DistancesMap> SpeciesDistancesMap;

  SortedDistanceComparisonData(const common::Structure & structure,
      const bool volumeAgnostic, const bool usePrimitive,
      const double cutoffFactor);

  ::std::vector< common::AtomSpeciesId::Value> species;
  SpeciesDistancesMap speciesDistancesMap;
  double cutoff;
  size_t numAtoms;
  double volume;

private:

  void
  initSpeciesDistancesMap();
};

class SortedDistanceComparator : public IStructureComparator
{
  typedef ::std::vector< double> DistancesVec;

public:
  typedef SortedDistanceComparisonData DataTyp;
  typedef IBufferedComparator BufferedTyp;
  typedef ::spl::UniquePtr< DataTyp>::Type ComparisonDataPtr;

  static const double DEFAULT_TOLERANCE;
  static const double DEFAULT_CUTOFF_FACTOR;
  static const bool DEFAULT_USE_PRIMITIVE;
  static const bool DEFAULT_VOLUME_AGNOSTIC;

  struct ConstructionInfo
  {
    ConstructionInfo()
    {
      tolerance = DEFAULT_TOLERANCE;
      cutoffFactor = DEFAULT_CUTOFF_FACTOR;
      usePrimitive = DEFAULT_USE_PRIMITIVE;
      volumeAgnostic = DEFAULT_VOLUME_AGNOSTIC;
    }
    double tolerance;
    double cutoffFactor;
    bool usePrimitive;
    bool volumeAgnostic;
  };

  SortedDistanceComparator();
  SortedDistanceComparator(const ConstructionInfo & info);

//  void
//  setCutoffFactor(const double cutoffFactor);
//  double
//  getCutoffFactor() const;

  // From IStructureComparator ////////////////

  virtual double
  compareStructures(const spl::common::Structure & str1,
      const spl::common::Structure & str2) const;
  virtual bool
  areSimilar(const spl::common::Structure & str1,
      const spl::common::Structure & str2) const;
  virtual ::boost::shared_ptr< BufferedTyp>
  generateBuffered() const;

  // End from IStructureComparator /////////////

  // Methods needed to conform to expectations laid out by GenericBufferedComparator ///
  double
  compareStructures(const SortedDistanceComparisonData & dist1,
      const SortedDistanceComparisonData & dist2) const;
  bool
  areSimilar(const SortedDistanceComparisonData & dist1,
      const SortedDistanceComparisonData & dist2) const;
  ComparisonDataPtr
  generateComparisonData(const ::spl::common::Structure & str) const;
  // End conformation methods //////////////

private:

  static const size_t MAX_CELL_MULTIPLES;

  void
  calcProperties(math::RunningStats & deltaStats, const DistancesVec & dist1,
      const StridedIndexAdapter< size_t> & adapt1, const DistancesVec & dist2,
      const StridedIndexAdapter< size_t> & adapt2) const;

  const bool myScaleVolumes;
  const bool myUsePrimitive;
  double myTolerance;
  double myCutoffFactor;
};

}
}

#endif /* SORTED_DISTANCE_COMPARATOR_H */
