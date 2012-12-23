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
#include "SSLib.h"

#include <vector>

#include <boost/shared_ptr.hpp>

#include "common/AtomSpeciesId.h"
#include "utility/IStructureComparator.h"
#include "utility/IndexAdapters.h"
#include "utility/MapEx.h"

// FORWARD DECLARATIONS ////////////////////////////////////
namespace sstbx {
namespace common {
class Structure;
}
}

namespace sstbx {
namespace utility {

class SortedDistanceComparisonData
{
public:
  typedef ::std::vector<double> DistancesVec;
  typedef ::boost::shared_ptr<DistancesVec> DistancesVecPtr;

  typedef utility::MapEx<common::AtomSpeciesId::Value, DistancesVecPtr> DistancesMap;
  typedef utility::MapEx<common::AtomSpeciesId::Value, DistancesMap> SpeciesDistancesMap;

  SortedDistanceComparisonData(const common::Structure & structure, const bool volumeAgnostic, const bool usePrimitive);

  ::std::vector<common::AtomSpeciesId::Value> species;
  SpeciesDistancesMap       speciesDistancesMap;
  double                    cutoff;
  size_t                    numAtoms;
  double                    volume;

private:

  void initSpeciesDistancesMap();
};

class SortedDistanceComparator : public IStructureComparator
{
  typedef ::std::vector<double> DistancesVec;
public:

	typedef SortedDistanceComparisonData DataTyp;
  typedef IBufferedComparator   BufferedTyp;
  typedef ::sstbx::UniquePtr<DataTyp>::Type ComparisonDataPtr;

	static const double DEFAULT_TOLERANCE;
  static const double CUTOFF_FACTOR;

	SortedDistanceComparator(double tolerance = DEFAULT_TOLERANCE, const bool volumeAgnostic = false, const bool usePrimitive = true);

  // From IStructureComparator ////////////////

	virtual double compareStructures(
		const sstbx::common::Structure & str1,
		const sstbx::common::Structure & str2) const;

	virtual bool areSimilar(
		const sstbx::common::Structure & str1,
		const sstbx::common::Structure & str2) const;

  virtual ::boost::shared_ptr<BufferedTyp> generateBuffered() const;

  // End from IStructureComparator /////////////

  // Methods needed to conform to expectations laid out by GenericBufferedComparator ///
	double compareStructures(
		const SortedDistanceComparisonData & dist1,
		const SortedDistanceComparisonData & dist2) const;

	bool areSimilar(
		const SortedDistanceComparisonData & dist1,
		const SortedDistanceComparisonData & dist2) const;

  ComparisonDataPtr generateComparisonData(const ::sstbx::common::Structure & str) const;
  // End conformation methods //////////////

private:

	static const size_t MAX_CELL_MULTIPLES;

  void calcProperties(
    const DistancesVec & dist1,
    const StridedIndexAdapter<size_t> & adapt1,
    const DistancesVec & dist2,
    const StridedIndexAdapter<size_t> & adapt2,
    double & sqSum,
    double & max,
    unsigned int & runningComparedTotal) const;

  const bool myScaleVolumes;
  const bool myUsePrimitive;
	double myTolerance;
};

}
}

#endif /* SORTED_DISTANCE_COMPARATOR_H */
