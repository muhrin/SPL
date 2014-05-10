/*
 * SortedDistanceComparatorEx.h
 *
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef SORTED_DISTANCE_COMPARATOR_EX_H
#define SORTED_DISTANCE_COMPARATOR_EX_H

// INCLUDES /////////////////////////////////////////////
#include "spl/SSLib.h"

#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "spl/common/AtomSpeciesId.h"
#include "spl/utility/IStructureComparator.h"
#include "spl/utility/MapEx.h"

// FORWARD DECLARATIONS ////////////////////////////////////
namespace spl {
namespace common {
class Structure;
}
}

namespace spl {
namespace utility {

class SortedDistanceComparisonDataEx
{
public:
  typedef ::std::vector<double> DistancesVec;;
  typedef ::boost::shared_ptr<DistancesVec> DistancesVecPtr;

  typedef utility::MapEx<common::AtomSpeciesId::Value, DistancesVecPtr> DistancesMap;
  typedef utility::MapEx<common::AtomSpeciesId::Value, DistancesMap> SpeciesDistancesMap;

  SortedDistanceComparisonDataEx(const common::Structure & structure, const double _cutoff);

  ::std::vector<common::AtomSpeciesId::Value> species;
  DistancesVec              latticeDistances;
  SpeciesDistancesMap       speciesDistancesMap;
  const double              cutoff;

private:

  void initSpeciesDistancesMap();
};

class SortedDistanceComparatorEx : public IStructureComparator
{
  typedef ::std::vector<double> DistancesVec;
public:

	typedef SortedDistanceComparisonDataEx DataTyp;
  typedef IBufferedComparator   BufferedTyp;
  typedef ::spl::UniquePtr<DataTyp>::Type ComparisonDataPtr;

	SortedDistanceComparatorEx(double tolerance = DEFAULT_TOLERANCE);

  // From IStructureComparator ////////////////

	virtual double compareStructures(
		const spl::common::Structure & str1,
		const spl::common::Structure & str2) const;

	virtual bool areSimilar(
		const spl::common::Structure & str1,
		const spl::common::Structure & str2) const;

  virtual ::boost::shared_ptr<BufferedTyp> generateBuffered() const;

  // End from IStructureComparator /////////////

  // Methods needed to conform to expectations laid out by GenericBufferedComparator ///
	double compareStructures(
		const SortedDistanceComparisonDataEx & dist1,
		const SortedDistanceComparisonDataEx & dist2) const;

	bool areSimilar(
		const SortedDistanceComparisonDataEx & dist1,
		const SortedDistanceComparisonDataEx & dist2) const;

  ComparisonDataPtr generateComparisonData(const ::spl::common::Structure & str) const;
  // End conformation methods //////////////
	

private:

	static const double DEFAULT_TOLERANCE;

	static const size_t MAX_CELL_MULTIPLES;

  void calcProperties(
    const DistancesVec & dist1,
    const DistancesVec & dist2,
    double & rms,
    double & max) const;

	double myTolerance;

};

}
}

#endif /* SORTED_DISTANCE_COMPARATOR_EX_H */
