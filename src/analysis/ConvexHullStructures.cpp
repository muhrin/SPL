/*
 * ConvexHullStructures.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#include "spl/analysis/ConvexHullStructures.h"

#ifdef SPL_WITH_CGAL

#define DEBUG_CONVEX_HULL_STRUCTURES

# include <boost/iterator/iterator_facade.hpp>

#include "spl/utility/TransformFunctions.h"

namespace spl {
namespace analysis {
namespace detail {

class StableStructuresIterator : public ::boost::iterator_facade<
    StableStructuresIterator, const common::Structure *,
    ::boost::forward_traversal_tag, const common::Structure *>
{
public:
  StableStructuresIterator() :
      myHullStructures(NULL)
  {
  }

  explicit
  StableStructuresIterator(const ConvexHullStructures * hullStructures) :
      myHullStructures(hullStructures)
  {
    myIt = myHullStructures->structuresBegin();
    goToFirstStable();
  }

private:

  const common::Structure *
  dereference() const
  {
    return *myIt;
  }

  void
  increment()
  {
    ++myIt;
    goToFirstStable();
  }

  bool
  equal(const StableStructuresIterator & other) const
  {
    if(isAtEnd() && other.isAtEnd())
      return true;

    return myIt == other.myIt;
  }

  void
  goToFirstStable()
  {
    // Find the next stable structure
    for(; myIt != myHullStructures->structuresEnd(); ++myIt)
    {
      if(myHullStructures->getStability(**myIt)
          == ConvexHullStructures::Stability::STABLE)
        break;
    }
  }

  bool
  isAtEnd() const
  {
    // If we don't have hull structures then we're an end iterator,
    // oterwise check if we're at the end of the hull structures
    return
        myHullStructures == NULL ?
            true : myIt == myHullStructures->structuresEnd();
  }

  const ConvexHullStructures * myHullStructures;
  ConvexHullStructures::StructuresIterator myIt;

  friend class boost::iterator_core_access;
};

} // namespace detail

ConvexHullStructures::ConvexHullStructures(const EndpointLabels & endpoints) :
    myConvexHull(endpoints)
{
}

ConvexHullStructures::ConvexHullStructures(const EndpointLabels & endpoints,
    utility::Key< double> & convexProperty) :
    myConvexHull(endpoints, convexProperty)
{
}

ConvexHullStructures::StructuresIterator
ConvexHullStructures::insert(common::Structure * structure)
{
  const ConvexHull::PointId id = myConvexHull.addStructure(*structure);
  if(id == -1)
    return structuresEnd();

  return StructuresIterator(
      myStructures.insert(::std::make_pair(structure, id)).first);
}

ConvexHullStructures::StructuresIterator
ConvexHullStructures::structuresBegin() const
{
  return StructuresIterator(myStructures.begin());
}

ConvexHullStructures::StructuresIterator
ConvexHullStructures::structuresEnd() const
{
  return StructuresIterator(myStructures.end());
}

size_t
ConvexHullStructures::numStable() const
{
  return ::std::distance(stableStructuresBegin(), stableStructuresEnd());
}

ConvexHullStructures::StableStructuresIterator
ConvexHullStructures::stableStructuresBegin() const
{
  return detail::StableStructuresIterator(this);
}

ConvexHullStructures::StableStructuresIterator
ConvexHullStructures::stableStructuresEnd() const
{
  return StableStructuresIterator();
}

const ConvexHull &
ConvexHullStructures::getHull() const
{
  return myConvexHull;
}

ConvexHullStructures::Stability::Value
ConvexHullStructures::getStability(const common::Structure & structure) const
{
  const Structures::const_iterator it = myStructures.find(
      const_cast< common::Structure *>(&structure));
  SSLIB_ASSERT(it != myStructures.end());

  if(it->second == -1)
    return Stability::NOT_HULL_STRUCTURE;
  else if(*myConvexHull.isStable(it->second))
    return Stability::STABLE;
  else
    return Stability::UNSTABLE;
}

double
ConvexHullStructures::getFormationEnthalpy(
    const StructuresIterator structure) const
{
  return getFormationEnthalpy(structure.base()->second);
}

void
ConvexHullStructures::populateFormationEnthalpies()
{
  BOOST_FOREACH(Structures::reference s, myStructures)
  {
    s.first->setProperty(
        common::structure_properties::general::FORMATION_ENTHALPY,
        getFormationEnthalpy(s.second));
  }
}

void
ConvexHullStructures::populateHullDistances()
{
  OptionalDouble dist;
  BOOST_FOREACH(Structures::reference s, myStructures)
  {
    dist = myConvexHull.distanceToHull(s.second);
    if(dist)
      s.first->setProperty(common::structure_properties::general::HULL_DISTANCE,
          *dist);
  }
}

OptionalDouble
ConvexHullStructures::distanceToHull(const common::Structure & structure) const
{
  const Structures::const_iterator it = myStructures.find(
      const_cast< common::Structure *>(&structure));
  if(it == myStructures.end())
    return myConvexHull.distanceToHull(structure);
  else
    return myConvexHull.distanceToHull(it->second); // It's faster to calculate if we know the id
}

::std::string
ConvexHullStructures::getLabel(const ConvexHull & convexHull,
    const ConvexHull::PointId pointId) const
{
  typedef utility::TakeSecond< const Structures::value_type> TakeSecond;
  typedef ::boost::transform_iterator< TakeSecond, Structures::const_iterator> IdsIterator;

  static const TakeSecond TAKE_SECOND = TakeSecond();

  SSLIB_ASSERT(&convexHull == &myConvexHull);

  const IdsIterator it = ::std::find(IdsIterator(myStructures.begin(), TAKE_SECOND),
      IdsIterator(myStructures.end(), TAKE_SECOND), pointId);

  if(it.base() == myStructures.end())
    return ::std::string();

  return it.base()->first->getComposition().toString();
}

double
ConvexHullStructures::getFormationEnthalpy(const ConvexHull::PointId id) const
{
  return CGAL::to_double(
      (*myConvexHull.getEntry(id).getPoint())[myConvexHull.dims() - 1]);
}

}
}

#endif // SPL_WITH_CGAL
