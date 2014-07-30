/*
 * ConvexHull.cpp
 *
 *  Created on: Nov 10, 2011
 *      Author: Martin Uhrin
 */

#include "spl/analysis/ConvexHull.h"

#ifdef SPL_USE_CGAL

#include <list>

#include <boost/foreach.hpp>

#include <CGAL/Delaunay_d.h>

#include "spl/common/StructureProperties.h"

//#define DEBUG_CONVEX_HULL_GENERATOR

namespace spl {
namespace analysis {

const ConvexHull::HullTraits::FT ConvexHull::FT_ZERO(0);
const ConvexHull::HullTraits::RT ConvexHull::RT_ZERO(0);

ConvexHull::ConvexHull(const EndpointLabels & labels) :
    myConvexProperty(common::structure_properties::general::ENTHALPY), myHullDims(
        labels.size())
{
  SSLIB_ASSERT_MSG(labels.size() >= 2,
      "Need at least two endpoints to make convex hull.");
  initEndpoints(labels);
}

ConvexHull::ConvexHull(const EndpointLabels & labels,
    utility::Key< double> & convexProperty) :
    myConvexProperty(convexProperty), myHullDims(labels.size())
{
  SSLIB_ASSERT_MSG(labels.size() >= 2,
      "Need at least two endpoints to make convex hull.");
  initEndpoints(labels);
}

ConvexHull::PointId
ConvexHull::addStructure(const common::Structure & structure)
{
  return generateEntry(structure);
}

int
ConvexHull::dims() const
{
  return myHullDims;
}

const ConvexHull::Hull *
ConvexHull::getHull() const
{
  if(!myHull.get() && canGenerate())
  {
    generateHull();
#ifdef DEBUG_CONVEX_HULL_GENERATOR
    myHull->print_statistics();
    SSLIB_ASSERT(myHull->is_valid());
#endif
  }

  return myHull.get();
}

ConvexHull::PointD
ConvexHull::composition(const PointD & point) const
{
  return PointD(myHullDims - 1, point.cartesian_begin(),
      point.cartesian_end() - 1);
}

ConvexHull::EndpointsConstIterator
ConvexHull::endpointsBegin() const
{
  return myEndpoints.begin();
}

ConvexHull::EndpointsConstIterator
ConvexHull::endpointsEnd() const
{
  return myEndpoints.end();
}

ConvexHull::EntriesConstIterator
ConvexHull::entriesBegin() const
{
  return myEntries.begin();
}

ConvexHull::EntriesConstIterator
ConvexHull::entriesEnd() const
{
  return myEntries.end();
}

const ConvexHull::HullEntry &
ConvexHull::getEntry(const PointId & id) const
{
  SSLIB_ASSERT_MSG(id >= 0 && id < static_cast< int>(myEntries.size()),
      "Invalid point id supplied: out of range");
  // Make sure the hull is generated
  getHull();

  return myEntries[id];
}

::boost::optional< bool>
ConvexHull::isStable(const PointD & point) const
{
#ifdef DEBUG_CONVEX_HULL_GENERATOR
  ::std::cout << "Checking stability of point: " << point << "\n";
#endif

  if(!myHull.get())
    return ::boost::optional< bool>();

  if(point[dims() - 1] > FT_ZERO)
    return false;

  // Have to do this special check for 1D as there is only one facet in this case
  // which is at the origin so anything along the line joining the two endpoints
  // would not be classed as 'stable' if we used the method below
  if(myHull->current_dimension() == 1)
    return myHull->bounded_side(point) == ::CGAL::ON_BOUNDARY;

  // TODO: Introduce hull facets that only includes facets other than
  // the top and any vertical facets

  const HullTraits::Oriented_side_d sideOf =
      HullTraits().oriented_side_d_object();
  HullTraits::Contained_in_simplex_d containedInSimplex =
      HullTraits().contained_in_simplex_d_object();

  HullTraits::Hyperplane_d hyperplane;
  BOOST_FOREACH(Hull::Facet_const_handle facet, myHull->all_facets())
  {
    if(isVerticalFacet(facet))
      continue;

    hyperplane = myHull->hyperplane_supporting(facet);
    if(sideOf(hyperplane, point) == CGAL::ON_ORIENTED_BOUNDARY
        && containedInSimplex(facet->points_begin() + 1,
            facet->points_begin() + myHull->current_dimension() + 1, point))
      return true;
  }
  return false;
}

::boost::optional< bool>
ConvexHull::isStable(const PointId id) const
{
  // Make sure the hull is generated
  getHull();

  const HullEntry & entry = myEntries[id];

  if(!entry.getPoint())
    return ::boost::optional< bool>();

  // Is it an endpoint on the hull?
  if(entry.isEndpoint() && (*entry.getPoint())[dims() - 1] == FT_ZERO)
    return true; // Skip the more complex check

  return isStable(*myEntries[id].getPoint());
}

OptionalDouble
ConvexHull::distanceToHull(const common::Structure & structure) const
{
  getHull(); // Make sure the hull is generated

  if(structure.getNumAtoms() == 0)
    return OptionalDouble();

  // Check if the structure has a value for the property that will form the 'depth' of the hull
  const double * const value = structure.getProperty(myConvexProperty);
  if(!value)
    return OptionalDouble();

  const PointD p = generateHullPoint(structure.getComposition(), *value);

  OptionalDouble dist;
  const ::boost::optional< HullTraits::RT> ftDist = distanceToHull(p);
  if(ftDist)
    dist.reset(CGAL::to_double(*ftDist));
  return dist;
}

OptionalDouble
ConvexHull::distanceToHull(const PointId id) const
{
  getHull(); // Make sure the hull is generated

  OptionalDouble dist;
  const ::boost::optional< HullTraits::RT> ftDist = distanceToHull(
      *myEntries[id].getPoint());
  if(ftDist)
    dist.reset(CGAL::to_double(*ftDist));
  return dist;
}

bool
ConvexHull::isVerticalFacet(Hull::Facet_const_handle facet) const
{
  const int currentDim = myHull->current_dimension();
  if(currentDim < myHullDims)
    return false; // No facet extends into the convex dimension

  const HullTraits::Direction_d direction =
      myHull->hyperplane_supporting(facet).orthogonal_direction();
  return direction[myHullDims - 1] == FT_ZERO;
}

ConvexHull::HullEntry::HullEntry(const common::AtomsFormula & composition,
    const HullTraits::FT value, const ConvexHull::PointId id,
    const bool isEndpoint) :
    myComposition(composition), myValue(value), myId(id), myIsEndpoint(
        isEndpoint)
{
}

const common::AtomsFormula &
ConvexHull::HullEntry::getComposition() const
{
  return myComposition;
}

const ConvexHull::HullTraits::FT
ConvexHull::HullEntry::getValue() const
{
  return myValue;
}

const ConvexHull::PointId &
ConvexHull::HullEntry::getId() const
{
  return myId;
}

const ::boost::optional< ConvexHull::PointD> &
ConvexHull::HullEntry::getPoint() const
{
  return myPoint;
}

bool
ConvexHull::HullEntry::isEndpoint() const
{
  return myIsEndpoint;
}

void
ConvexHull::HullEntry::setPoint(const PointD & p)
{
  myPoint = p;
}

ConvexHull::PointId
ConvexHull::generateEntry(const common::Structure & structure)
{
  if(structure.getNumAtoms() == 0)
    return -1;

  // Check if the structure has a value for the property that will form the 'depth' of the hull
  const double * const value = structure.getProperty(myConvexProperty);
  if(!value)
    return -1;

  // Check that the structure contains at least one endpoint
  common::AtomsFormula composition = structure.getComposition();
  bool containsEndpoint = false, isEndpoint = true;
  int formulaUnits, endpointFormulaUnits;
  common::AtomsFormula endpoint;
  BOOST_FOREACH(Endpoints::const_reference e, myEndpoints)
  {
    formulaUnits = composition.wholeNumberOf(e.first);

    if(formulaUnits != 0)
    {
      if(containsEndpoint)
        isEndpoint = false; // Contains more than one endpoint
      else
      {
        containsEndpoint = true; // The first endpoint we've come across so far
        endpointFormulaUnits = formulaUnits;
        endpoint = e.first;
      }
      composition.remove(e.first, formulaUnits);
    }
  }
  if(!containsEndpoint)
    return -1; // Doesn't contain any endpoints so it's nowhere in relation to hull
  if(!composition.isEmpty())
    return -1; // This structure has atoms that aren't on the hull so we can't calculate the chemical potential

  const PointId id = myEntries.size();
  myEntries.insert(myEntries.end(),
      HullEntry(structure.getComposition(), *value, id, isEndpoint));
  if(isEndpoint)
    updateChemicalPotential(endpoint,
        HullTraits::FT(*value) / HullTraits::FT(endpointFormulaUnits));

  return id;
}

void
ConvexHull::updateChemicalPotential(
    const common::AtomsFormula & endpointFormula, const HullTraits::FT value)
{
  ChemicalPotentials::iterator it = myChemicalPotentials.find(endpointFormula);
  if(it == myChemicalPotentials.end())
    myChemicalPotentials[endpointFormula] = value;
  else
  {
    if(it->second > value)
    {
#ifdef DEBUG_CONVEX_HULL_GENERATOR
      ::std::cout << "Updating endpoint " << endpointFormula << " to " << value << ::std::endl;
#endif
      it->second = value;
      myHull.reset(); // Hull needs to be re-generated
    }
  }
}

ConvexHull::PointD
ConvexHull::generateHullPoint(const HullEntry & entry) const
{
  PointD point = generateHullPoint(entry.getComposition(), entry.getValue());
  point.setId(entry.getId());
  return point;
}

ConvexHull::PointD
ConvexHull::generateHullPoint(const common::AtomsFormula & composition,
    const HullTraits::FT & convexValue) const
{
  // Need chemical potentials for all endpoints
  SSLIB_ASSERT(canGenerate());

  HullTraits::FT totalMuNAtoms = FT_ZERO;
  HullTraits::FT totalAtoms, numAtoms;
  BOOST_FOREACH(Endpoints::const_reference endpoint, myEndpoints)
  {
    numAtoms = composition.wholeNumberOf(endpoint.first);
    if(numAtoms != 0)
    {
      totalAtoms += numAtoms;
      totalMuNAtoms += myChemicalPotentials.find(endpoint.first)->second
          * HullTraits::FT(numAtoms);
    }
  }

  // Create a vector that is the weighted sum of the vectors of composition
  // simplex
  ::std::vector< HullTraits::FT> tempVec(myHullDims);
  VectorD v(myHullDims);
  int numEndpoints = 0;
  for(size_t i = 0; i < myEndpoints.size(); ++i)
  {
    numAtoms = composition.wholeNumberOf(myEndpoints[i].first);
    if(numAtoms != 0)
    {
      VectorD scaled = myEndpoints[i].second - CGAL::ORIGIN;
#ifdef DEBUG_CONVEX_HULL_GENERATOR
      ::std::cout << "Adding weighted hull endpoint: " << scaled << " weight: " << numAtoms / totalAtoms << ::std::endl;
#endif
      scaled *= numAtoms / totalAtoms;
      v += scaled;
      ++numEndpoints;
    }
  }
  tempVec.assign(v.cartesian_begin(), v.cartesian_end());
  // The last hull coordinate is always the 'convex property', usually the enthalpy
  tempVec[dims() - 1] = (convexValue - totalMuNAtoms) / totalAtoms;

  PointD point(myHullDims, tempVec.begin(), tempVec.end());

#ifdef DEBUG_CONVEX_HULL_GENERATOR
  ::std::cout << "Convex value = " << convexValue << " mu_alpha N_alpha = " << totalMuNAtoms << " total atoms = " << totalAtoms << ::std::endl;
  ::std::cout << "Point entry = " << point << ::std::endl;
#endif

  return point;
}

void
ConvexHull::generateHull() const
{
  SSLIB_ASSERT(canGenerate());

  typedef ::std::map< common::AtomsFormula, HullEntry *> LowestEnergy;
  // Need to allow multiple structures with the same formation enthalpy so long
  // as they have different compositions
  typedef ::std::multimap< HullTraits::FT, HullEntry *> SortedEntries;

  // To make calculating the hull faster and remove redundant points
  // first get the set of points with the lowest energy at composition coordinate
  LowestEnergy lowest;
  LowestEnergy::iterator it;
  BOOST_FOREACH(HullEntries::reference entry, myEntries)
  {
    const PointD & p = generateHullPoint(entry);
    entry.setPoint(p);

    if(p[dims() - 1] <= FT_ZERO)
    {
      common::AtomsFormula composition = entry.getComposition();
      composition.reduce();
      it = lowest.find(composition);
      if(it == lowest.end())
        lowest[composition] = &entry;
      else
      {
        // If this point is lower in energy then replace the old one
        if(p[dims() - 1] < (*it->second->getPoint())[dims() - 1])
          it->second = &entry;
      }
    }
  }

  // Now sort the remaining entries by energy
  SortedEntries sortedEntries;
  BOOST_FOREACH(LowestEnergy::reference entry, lowest)
  {
    sortedEntries.insert(
        ::std::make_pair((*entry.second->getPoint())[dims() - 1],
            entry.second));
  }

  myHull.reset(new Hull(myHullDims));
  // Put in the endpoints first
  BOOST_FOREACH(Endpoints::const_reference ep, myEndpoints)
  {
    myHull->insert(ep.second);
  }
  // Put the points into the hull from lowest up
  BOOST_FOREACH(SortedEntries::const_reference entry, sortedEntries)
  {
    if(!entry.second->isEndpoint())
      myHull->insert(*entry.second->getPoint());
  }

#ifdef DEBUG_CONVEX_HULL_GENERATOR
  ::std::cout << "Original hull statistics:\n";
  myHull->print_statistics();
#endif

  // Get rid of any points that lie on facets that are vertical wrt convex dimension
  ::std::vector< PointD> toCheck, toKeep;
  toCheck.reserve(myHull->number_of_vertices());
  toKeep.reserve(myHull->number_of_vertices());

  for(Hull::Hull_point_const_iterator it = myHull->hull_points_begin(), end =
      myHull->hull_points_end(); it != end; ++it)
    toCheck.push_back(*it);

  Hull::Hyperplane_d hyperplane;
  const HullTraits::Oriented_side_d sideOf =
      HullTraits().oriented_side_d_object();
  for(Hull::Facet_const_iterator facet =
      const_cast< const Hull *>(myHull.get())->facets_begin(), facetEnd =
      const_cast< const Hull *>(myHull.get())->facets_end(); facet != facetEnd;
      ++facet)
  {
    if(isVerticalFacet(facet))
      continue;

    hyperplane = myHull->hyperplane_supporting(facet);
    for(::std::vector< PointD>::iterator it = toCheck.begin();
        it != toCheck.end();
        /* increment in loop */)
    {
      if(sideOf(hyperplane, *it) == CGAL::ON_ORIENTED_BOUNDARY)
      {
        toKeep.push_back(*it);
        it = toCheck.erase(it);
      }
      else
        ++it;
    }
  }

  // Regenerate the hull
  myHull.reset(new Hull(myHullDims));
  myHull->insert(toKeep.begin(), toKeep.end());

#ifdef DEBUG_CONVEX_HULL_GENERATOR
  ::std::cout << "Keeping:\n";
  BOOST_FOREACH(const PointD & p, toKeep)
  ::std::cout << p << ::std::endl;
#endif
}

bool
ConvexHull::canGenerate() const
{
  return myChemicalPotentials.size() == myEndpoints.size();
}

void
ConvexHull::initEndpoints(const EndpointLabels & labels)
{
  for(size_t i = 0; i < labels.size(); ++i)
  {
    myEndpoints.push_back(::std::make_pair(labels[i], PointD(myHullDims)));
    // Make sure we're using the reduced, i.e. most simple, formula
    // e.g. A4B2 => A2B
    myEndpoints.back().first.reduce();
  }

  ::std::vector< RT> vec(myHullDims, 0);

  // Always start the hull with the first points at (0,...) and (1,...)
  vec[0] = 1;
  myEndpoints[1].second = PointD(vec.size(), vec.begin(), vec.end());

#ifdef DEBUG_CONVEX_HULL_GENERATOR
  ::std::cout << "Convex hull building simplex\n";
  ::std::cout << "0: " << myEndpoints[0].second << ::std::endl;
  ::std::cout << "1: " << myEndpoints[1].second << ::std::endl;
#endif

  PointD pSum = myEndpoints[1].second;
  vec.assign(vec.size(), RT_ZERO);
  for(int i = 2; i < myHullDims; ++i)
  {
    // Put the new point at the centre of the previous points
    // and 'raise' the last coordinate into the new dimension
    // such as to make the new point unit distance from all the other points
    vec[i - 1] = 1.0;
    for(int j = 0; j < i - 1; ++j)
    {
      vec[j] = pSum[j] / i;
      vec[i - 1] -= vec[j] * vec[j];
    }
    vec[i - 1] = CGAL::sqrt(vec[i - 1]);
    myEndpoints[i].second = PointD(vec.size(), vec.begin(), vec.end());

#ifdef DEBUG_CONVEX_HULL_GENERATOR
    ::std::cout << i << ": " << CGAL::to_double(myEndpoints[i].second[0]) << " " << CGAL::to_double(myEndpoints[i].second[1]) << ::std::endl;
#endif

    pSum += myEndpoints[i].second - CGAL::ORIGIN;
  }
#ifdef DEBUG_CONVEX_HULL_GENERATOR
  ::std::cout << "Convex hull finished building simplex\n";
#endif
}

::boost::optional< ConvexHull::HullTraits::RT>
ConvexHull::distanceToHull(const PointD & p) const
{
  if(!getHull())
    return ::boost::optional< ConvexHull::HullTraits::RT>();

  SSLIB_ASSERT_MSG(p.dimension() == myHullDims,
      "Point must have same dimension as hull to calculate distance.");

#ifdef DEBUG_CONVEX_HULL_GENERATOR
  ::std::cout << "Calculating distance to point: ";
  printPoint(p);
#endif

  // Endpoints are anchored to 0 so the distances is just the convex value of this point
  // also if the hull only contains endpoints then it will be a plane (line) at 0 in the
  // convex dimensions so the convex value is just the height above the plane
  if(isEndpoint(p) || containsOnlyEndpoints())
    return p[myHullDims - 1];

  // Create the line to test against
  ::std::vector< HullTraits::FT> direction(myHullDims, FT_ZERO);
  direction[myHullDims - 1] = 1.0;
  HullTraits::Line_d line(p,
      HullTraits::Direction_d(myHullDims, direction.begin(), direction.end()));

  // Test objects
  HullTraits::Contained_in_simplex_d containedInSimplex =
      HullTraits().contained_in_simplex_d_object();
  HullTraits::Intersect_d intersect = HullTraits().intersect_d_object();

  Hull::Hyperplane_d hyperplane;
  for(Hull::Facet_const_iterator it =
      const_cast< const Hull *>(myHull.get())->facets_begin(), end =
      const_cast< const Hull *>(myHull.get())->facets_end(); it != end; ++it)
  {
    if(isTopFacet(it) || isVerticalFacet(it))
      continue;

    hyperplane = myHull->hyperplane_supporting(it);

    ::CGAL::Object result = intersect(line, hyperplane);

    // TODO: Change kernel to use our point type
    const CGAL::Cartesian_d< RT>::Point_d * const intersectionPoint =
        CGAL::object_cast< CGAL::Cartesian_d< RT>::Point_d>(&result);
    if(intersectionPoint)
    {
      const PointD interPoint = PointD(myHullDims,
          intersectionPoint->cartesian_begin(),
          intersectionPoint->cartesian_end());
      // Have to start at points_begin() + 1 because this is the way simplices represent facets.
      if(containedInSimplex(it->points_begin() + 1,
          it->points_begin() + myHull->current_dimension() + 1, interPoint))
      {
#ifdef DEBUG_CONVEX_HULL_GENERATOR
        ::std::cout << "Found distance to point, from = " << p << ", to = " << interPoint << ::std::endl;
        ::std::cout << "Hyperplane: " << *it << ::std::endl;
#endif
        return p[myHullDims - 1] - interPoint[myHullDims - 1];
      }
    }
  }

  return ::boost::optional< RT>(); // Point was not in the space of the hull
}

bool
ConvexHull::isTopFacet(Hull::Facet_const_iterator facet) const
{
  for(int i = 0; i < myHull->current_dimension(); ++i)
  {
    if(!isEndpoint(myHull->point_of_facet(facet, i)))
      return false;
  }
  return true;
}

bool
ConvexHull::isEndpoint(const PointD & p) const
{
  BOOST_FOREACH(const Endpoint & ep, myEndpoints)
  {
    if(p == ep.second)
      return true;
  }
  return false;
}

bool
ConvexHull::containsOnlyEndpoints() const
{
#ifdef DEBUG_CONVEX_HULL_GENERATOR
  // Conveniences for debugging
  const size_t numVertices = getHull()->number_of_vertices();
  const size_t numEndpoints = myEndpoints.size();
#endif

  return getHull()->number_of_vertices() == myEndpoints.size();
}

void
ConvexHull::printPoint(const PointD & p) const
{
  for(int i = 0; i < p.dimension(); ++i)
    ::std::cout << CGAL::to_double(p[i]) << " ";
  ::std::cout << ::std::endl;
}

}
}

#endif // SPL_USE_CGAL
