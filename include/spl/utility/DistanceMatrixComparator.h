/*
 * DistanceMatrixComparator.h
 *
 *
 *  Created on: Aug 29, 2012
 *      Author: Martin Uhrin
 */

#ifndef DISTANCE_MATRIX_COMPARATOR_H
#define DISTANCE_MATRIX_COMPARATOR_H

// INCLUDES /////////////////////////////////////////////
#include "spl/SSLib.h"

#include <map>
#include <set>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <armadillo>

#include "spl/common/AtomSpeciesId.h"
#include "spl/utility/IStructureComparator.h"

// FORWARD DECLARATIONS ////////////////////////////////////
namespace spl {
namespace common {
class Structure;
}
}

namespace spl {
namespace utility {

struct DistanceMatrixComparisonData
{
  typedef ::std::map< common::AtomSpeciesId::Value, ::std::vector< size_t> > SortedIndices;

  DistanceMatrixComparisonData(const common::Structure & structure);

  ::arma::mat distancesMtx;
  ::std::vector< common::AtomSpeciesId::Value> speciesList;
  ::std::set< common::AtomSpeciesId::Value> speciesSet;
};

class DistanceMatrixComparator : public IStructureComparator
{
public:
  typedef DistanceMatrixComparisonData DataTyp;
  typedef UniquePtr< DataTyp>::Type ComparisonDataPtr;

  static const double STRUCTURES_INCOMPARABLE;
  static const double DEFAULT_TOLERANCE;

  // fastComparisonAtomLimit - above this many atoms use fast comparison method as
  // the number of permutations (N!) will be too large.
  DistanceMatrixComparator(const size_t fastComparisonAtomsLimit = 12);
  DistanceMatrixComparator(const double tolerance,
      const size_t fastComparisonAtomsLimit = 12);

  // From IStructureComparator ////////////////

  virtual double
  compareStructures(const common::Structure & str1,
      const common::Structure & str2) const;

  virtual bool
  areSimilar(const common::Structure & str1,
      const common::Structure & str2) const;

  virtual ::boost::shared_ptr< IBufferedComparator>
  generateBuffered() const;

  // End from IStructureComparator /////////////

  // Methods needed to conform to expectations laid out by GenericBufferedComparator ///
  bool
  areSimilar(const DataTyp & str1Data, const DataTyp & str2Data) const;

  double
  compareStructures(const DataTyp & str1Data, const DataTyp & str2Data) const;

  ComparisonDataPtr
  generateComparisonData(const common::Structure & structure) const;
  // End conformation methods //////////////

private:

  bool
  areComparable(const DataTyp & str1Data, const DataTyp & str2Data) const;

  double
  compareStructuresFull(const DataTyp & str1Data,
      const DataTyp & str2Data) const;

  double
  compareStructuresFast(const DataTyp & str1Data,
      const DataTyp & str2Data) const;

  const size_t myFastComparisonAtomsLimit;
  const double myTolerance;
};

}
}

#endif /* DISTANCE_MATRIX_COMPARATOR_H */
