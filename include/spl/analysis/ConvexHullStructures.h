/*
 * ConvexHullStructures.h
 *
 *  Created on: Aug 17, 2011
 *      Author: Martin Uhrin
 */

#ifndef STABLE_COMPOSITIONS_H
#define STABLE_COMPOSITIONS_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_USE_CGAL

#include <map>

#include <boost/iterator/indirect_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "spl/analysis/ConvexHull.h"
#include "spl/analysis/ConvexHullInfoSupplier.h"
#include "spl/utility/HeterogeneousMapKey.h"
#include "spl/utility/TransformFunctions.h"

// DEFINITION ///////////////////////

namespace spl {

// FORWARD DECLARATIONS ///////
namespace common {
class Structure;
}

namespace analysis {
namespace detail {
class StableStructuresIterator;
}

class ConvexHullStructures : public ConvexHullInfoSupplier
{
  typedef ::std::map< common::Structure *, ConvexHull::PointId> Structures;
  typedef utility::TakeFirst< const Structures::value_type> TakeFirst;

public:
  typedef ConvexHull::EndpointLabels EndpointLabels;
  typedef ::boost::transform_iterator< TakeFirst, Structures::const_iterator> StructuresIterator;
  typedef detail::StableStructuresIterator StableStructuresIterator;

  struct Stability
  {
    enum Value
    {
      UNSTABLE, STABLE, NOT_HULL_STRUCTURE
    };
  };

  template< typename InputIterator>
    static EndpointLabels
    generateEndpoints(InputIterator first, InputIterator last);

  ConvexHullStructures(const EndpointLabels & endpoints);
  ConvexHullStructures(const EndpointLabels & endpoints,
      utility::Key< double> & convexProperty);
  template< typename InputIterator>
    ConvexHullStructures(InputIterator first, InputIterator last);

  StructuresIterator
  insert(common::Structure * const structure);
  template< typename InputIterator>
    void
    insert(InputIterator first, InputIterator last);

  StructuresIterator
  structuresBegin() const;
  StructuresIterator
  structuresEnd() const;

  size_t
  numStable() const;
  StableStructuresIterator
  stableStructuresBegin() const;
  StableStructuresIterator
  stableStructuresEnd() const;

  const ConvexHull &
  getHull() const;

  Stability::Value
  getStability(const common::Structure & structure) const;

  double
  getFormationEnthalpy(const StructuresIterator structure) const;
  void
  populateFormationEnthalpies();
  void
  populateHullDistances();

  OptionalDouble
  distanceToHull(const common::Structure & structure) const;

  virtual ::std::string
  getLabel(const ConvexHull & convexHull,
      const ConvexHull::PointId pointId) const;

private:
  double
  getFormationEnthalpy(const ConvexHull::PointId id) const;

  ConvexHull myConvexHull;
  Structures myStructures;
};

template< typename InputIterator>
  ConvexHullStructures::ConvexHullStructures(InputIterator first,
      InputIterator last) :
      myConvexHull(
          ConvexHull::generateEndpoints(::boost::make_indirect_iterator(first),
              ::boost::make_indirect_iterator(last)))
  {
    insert(first, last);
  }

template< typename InputIterator>
  void
  ConvexHullStructures::insert(InputIterator first, InputIterator last)
  {
    for(InputIterator it = first; it != last; ++it)
      insert(*it);
  }

template< typename InputIterator>
  ConvexHullStructures::EndpointLabels
  ConvexHullStructures::generateEndpoints(InputIterator first,
      InputIterator last)
  {
    return ConvexHull::generateEndpoints(::boost::make_indirect_iterator(first),
        ::boost::make_indirect_iterator(last));
  }

}
}

#include "spl/analysis/detail/ConvexHullStructures.h"

#endif // SPL_USE_CGAL
#endif /* STABLE_COMPOSITIONS_H */
