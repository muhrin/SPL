/*
 * VoronoiSlabRegions.cpp
 *
 *  Created on: Jan 17, 2014
 *      Author: Martin Uhrin
 */

// INCLUEDES /////////////
#include "spl/build_cell/VoronoiSlabRegions.h"

#ifdef SPL_WITH_CGAL

#include <boost/foreach.hpp>

#include "spl/common/Atom.h"
#include "spl/math/Geometry.h"
#include "spl/math/NumberAlgorithms.h"
#include "spl/math/Random.h"
#include "spl/utility/Armadillo.h"

namespace spl {
namespace build_cell {

using arma::endr;

LatticeRegion::LatticeRegion(const ConstructionInfo & info,
    UniquePtr< Basis>::Type & basis) :
    SlabRegion(info.boundary, basis), myStartPoint(info.startPoint), myFixPoints(
        info.fixPoints ? *info.fixPoints : true)
{
  myLattice[0] = info.vecA;
  myLattice[1] = info.vecB;
}

LatticeRegion::LatticeRegion(const LatticeRegion & toCopy) :
    SlabRegion(toCopy), myLattice(toCopy.myLattice), myStartPoint(
        toCopy.myStartPoint), myFixPoints(toCopy.myFixPoints)
{
}

LatticeRegion::Ticket
LatticeRegion::generateSites(Delaunay * const dg) const
{
  const CGAL::Bbox_2 & bbox = getBoundary().bbox();
  arma::vec2 x0;
  if(myStartPoint)
    x0 = *myStartPoint;
  else
    x0 << bbox.xmin() << endr << bbox.ymin(); // Use the bottom left corner of the AABB

  // Find out the min and max multiples of each lattice vector needed to cover
  // the bounding box entirely.
  const LatticeMultiples latVec0Multiples = getLatticeMultiples(x0,
      myLattice[0], myLattice[1]);
  const LatticeMultiples latVec1Multiples = getLatticeMultiples(x0,
      myLattice[1], myLattice[0]);

  for(int i = latVec0Multiples.min(); i <= latVec0Multiples.max(); ++i)
  {
    const arma::vec2 & r = x0 + static_cast< double>(i) * myLattice[0];
    generateLine(r, myLattice[1], latVec1Multiples, bbox, dg);
  }

  return Ticket();
}

std::auto_ptr< VoronoiSlabGenerator::SlabRegion>
LatticeRegion::clone() const
{
  return std::auto_ptr< VoronoiSlabGenerator::SlabRegion>(
      new LatticeRegion(*this));
}

LatticeRegion::LatticeMultiples
LatticeRegion::getLatticeMultiples(const arma::vec2 & r0,
    const arma::vec2 & latticeVecP, const arma::vec2 & latticeVecQ) const
{
  using arma::dot;
  using std::ceil;
  using std::max;
  using std::min;
  using std::sqrt;

  const CGAL::Bbox_2 & bbox = getBoundary().bbox();
  const math::Line2 pLine(r0, r0 + latticeVecP);

  int x0 = std::numeric_limits< int>::max();
  int x1 = std::numeric_limits< int>::min();
  // Go around all four corners of the bounding box
  arma::vec boxPt;
  { // bottom left
    boxPt << bbox.xmin() << endr << bbox.ymin();

    const double numMultiples = getLatticeVecMultiple(r0, latticeVecP, pLine,
        math::Line2(boxPt, boxPt + latticeVecQ));

    x0 = min(x0, static_cast< int>(math::roundAwayFromZero(numMultiples)));
    x1 = max(x1, static_cast< int>(math::roundAwayFromZero(numMultiples)));
  }
  { // top left
    boxPt << bbox.xmin() << endr << bbox.ymax();

    const double numMultiples = getLatticeVecMultiple(r0, latticeVecP, pLine,
        math::Line2(boxPt, boxPt + latticeVecQ));

    x0 = min(x0, static_cast< int>(math::roundAwayFromZero(numMultiples)));
    x1 = max(x1, static_cast< int>(math::roundAwayFromZero(numMultiples)));
  }
  { // top right
    boxPt << bbox.xmax() << endr << bbox.ymax();

    const double numMultiples = getLatticeVecMultiple(r0, latticeVecP, pLine,
        math::Line2(boxPt, boxPt + latticeVecQ));

    x0 = min(x0, static_cast< int>(math::roundAwayFromZero(numMultiples)));
    x1 = max(x1, static_cast< int>(math::roundAwayFromZero(numMultiples)));
  }
  { // bottom right
    boxPt << bbox.xmax() << endr << bbox.ymin();

    const double numMultiples = getLatticeVecMultiple(r0, latticeVecP, pLine,
        math::Line2(boxPt, boxPt + latticeVecQ));

    x0 = min(x0, static_cast< int>(math::roundAwayFromZero(numMultiples)));
    x1 = max(x1, static_cast< int>(math::roundAwayFromZero(numMultiples)));
  }

  return LatticeMultiples(x0, x1);
}

int
LatticeRegion::getLatticeVecMultiple(const arma::vec2 & x0,
    const arma::vec2 & latticeVec, const math::Line2 & latticeLine,
    const math::Line2 & intersectionLine) const
{
  using arma::dot;
  using std::ceil;
  using std::sqrt;

  const arma::vec2 & dr = math::intersectionPoint(latticeLine, intersectionLine)
      - x0;

  return math::sgn(dot(dr, latticeVec))
      * ceil(sqrt(dot(dr, dr) / dot(latticeVec, latticeVec)));
}

void
LatticeRegion::generateLine(const arma::vec2 & r0, const arma::vec2 & dr,
    const LatticeMultiples & range, const CGAL::Bbox_2 & box,
    Delaunay * const dg) const
{
  arma::vec2 r;
  for(int i = range.min(); i <= range.max(); ++i)
  {
    r = r0 + static_cast< double>(i) * dr;
    if(inBBox(r, box) && withinBoundary(r))
    {
      Delaunay::Vertex_handle vtx = dg->insert(utility::toCgalPoint< K>(r));
      vtx->info().fixed = myFixPoints;
      vtx->info().generatedBy = this;
    }
  }
}

bool
LatticeRegion::inBBox(const arma::vec2 & r, const CGAL::Bbox_2 & bbox) const
{
  return r(0) >= bbox.xmin() && r(0) <= bbox.xmax() && r(1) >= bbox.ymin()
      && r(1) <= bbox.ymax();
}

RandomRegion::RandomRegion(const std::vector< arma::vec2> & boundary,
    const int numPoints, const double minsep, UniquePtr< Basis>::Type & basis) :
    SlabRegion(boundary, basis), myNumPoints(numPoints), myMinsep(minsep), myPolyMode(
        PolyMode::FREE)
{
}

RandomRegion::Ticket
RandomRegion::generateSites(Delaunay * const dg) const
{
  generateSites(myNumPoints, dg);

  Ticket ticket;

  switch(myPolyMode)
  {
  case PolyMode::FIXED:
    if(!myPolyNumbers.empty())
      ticket = myPolyPool.insert(myPolyNumbers);
    break;
  case PolyMode::STOCHASTIC:
    // TODO: Generate weighted probability numbers
    break;
  case PolyMode::FREE:
  default:
    break;
  }

  return ticket;
}

bool
RandomRegion::refine(const Ticket & ticket,
    const std::set< Delaunay::Vertex_handle> & vertices,
    Delaunay * const dg) const
{
  typedef std::vector< Delaunay::Vertex_handle> OldVertices;
  typedef std::vector< std::pair< Delaunay::Point, Delaunay::Vertex::Info> > NewVertices;

  const PolygonNumbers & target = myPolyPool.get(ticket);
  SSLIB_ASSERT(!target.empty());

  bool madeChange = false;
  int degree;
  OldVertices toRemove;
  NewVertices toInsert;
  PolygonNumbers diffs = getPolyDiffs(*dg, vertices, target);
  PolygonNumbers::iterator diff;
  for(std::set< Delaunay::Vertex_handle>::iterator it = vertices.begin();
      it != vertices.end(); /*increment in loop*/)
  {
    std::set< Delaunay::Vertex_handle>::iterator next = it;
    ++next;

    if(!(*it)->info().fixed && !detail::isBoundaryVertex(*dg, *it))
    {
      degree = (*it)->degree();
      diff = diffs.find(degree);
      SSLIB_ASSERT(diff != diffs.end());
      if(diff->second < 0)
      {
        toRemove.push_back(*it);
        ++diff->second;
        madeChange = true;
      }
      else if(diff->second > 0)
      {
        Delaunay::Point pt((*it)->point().x(),
            CGAL::to_double((*it)->point().y() + 0.1));
        toInsert.push_back(std::make_pair(pt, (*it)->info()));
        --diff->second;
        madeChange = true;
      }
    }

    it = next;
  }

  BOOST_FOREACH(OldVertices::const_reference vtx, toRemove)
    dg->remove(vtx);

  BOOST_FOREACH(NewVertices::const_reference pt, toInsert)
  {
    const Delaunay::Vertex_handle vtx = dg->insert(pt.first);
    vtx->info() = pt.second;
  }

  const int toGenerate = myNumPoints - vertices.size() - toRemove.size()
      + toInsert.size();
  if(toGenerate > 0)
    generateSites(toGenerate, dg);

  return madeChange;
}

bool
RandomRegion::good(const Ticket & ticket,
    const std::set< Delaunay::Vertex_handle> & vertices,
    const Delaunay & dg) const
{
  const PolygonNumbers & target = myPolyPool.get(ticket);
  const PolygonNumbers & current = measurePolyNumbers(dg, vertices);

  if(target.size() != current.size())
    return false;

  // Check the types of polygons present and make sure there isn't a mismatch
  // and calculate the totals
  int targetTotal = 0, currentTotal = 0;
  bool mismatch = false;
  for(PolygonNumbers::const_iterator tIt = target.begin(), tEnd = target.end(),
      cIt = current.begin(), cEnd = current.end(); tIt != tEnd; ++tIt, ++cIt)
  {
    if(tIt->first != cIt->first)
    {
      mismatch = true;
      break;
    }
    targetTotal += tIt->second;
    currentTotal += cIt->second;
  }
  if(mismatch)
    return false;

  // Now check that the ratios are the same (within +/- 0.5 an integer)
  const double delta = std::max(1.0 / static_cast< double>(2 * targetTotal),
      1.0 / static_cast< double>(2 * currentTotal));
  double tRatio, cRatio;
  for(PolygonNumbers::const_iterator tIt = target.begin(), tEnd = target.end(),
      cIt = current.begin(), cEnd = current.end(); tIt != tEnd; ++tIt, ++cIt)
  {
    tRatio = static_cast< double>(tIt->second)
        / static_cast< double>(targetTotal);
    cRatio = static_cast< double>(cIt->second)
        / static_cast< double>(currentTotal);
    if(cRatio < tRatio - delta || cRatio > tRatio + delta)
    {
      mismatch = true;
      break;
    }
  }
  if(mismatch)
    return false;

  return true;
}

void
RandomRegion::generateSites(const size_t num, Delaunay * const dg) const
{
  const CGAL::Bbox_2 & bbox = getBoundary().bbox();
  arma::vec2 pt;
  for(int numGenerated = 0; numGenerated < myNumPoints; /*increment in body*/)
  {
    pt << math::randu(bbox.xmin(), bbox.xmax()) << endr
        << math::randu(bbox.ymin(), bbox.ymax());
    if(withinBoundary(pt))
    {
      Delaunay::Vertex_handle vtx = dg->insert(utility::toCgalPoint< K>(pt));
      vtx->info().fixed = false;
      vtx->info().minsep = myMinsep;
      vtx->info().generatedBy = this;
      ++numGenerated;
    }
  }
}

RandomRegion::PolygonNumbers
RandomRegion::measurePolyNumbers(const Delaunay & dg,
    const std::set< Delaunay::Vertex_handle> & vertices) const
{
  PolygonNumbers numbers;
  std::pair< PolygonNumbers::iterator, bool> insertRet;
  int degree;
  BOOST_FOREACH(const Delaunay::Vertex_handle & vtx, vertices)
  {
    // Only interested in non-boundary polygons
    if(!detail::isBoundaryVertex(dg, vtx))
    {
      degree = vtx->degree();
      insertRet = numbers.insert(std::make_pair(degree, 1));
      if(insertRet.second == false)
        ++insertRet.first->second;
    }
  }
  return numbers;
}

RandomRegion::PolygonNumbers
RandomRegion::getPolyDiffs(const Delaunay & dg,
    const std::set< Delaunay::Vertex_handle> & vertices,
    const PolygonNumbers & target) const
{
  const PolygonNumbers & current = measurePolyNumbers(dg, vertices);

  int targetTotal = 0, currentTotal = 0;
  BOOST_FOREACH(PolygonNumbers::const_reference n, target)
    targetTotal += n.second;
  BOOST_FOREACH(PolygonNumbers::const_reference n, current)
    currentTotal += n.second;

  const double multiplier = static_cast< double>(currentTotal)
      / static_cast< double>(targetTotal);
  PolygonNumbers::const_iterator it;
  PolygonNumbers diffs;
  BOOST_FOREACH(PolygonNumbers::const_reference n, current)
  {
    it = target.find(n.first);
    if(it == target.end())
      diffs[n.first] = -n.second; // Get rid of all of them
    else
      diffs[n.first] = static_cast< int>((static_cast< double>(it->second)
          * multiplier) - static_cast< double>(n.second));
  }
  return diffs;
}

std::auto_ptr< VoronoiSlabGenerator::SlabRegion>
RandomRegion::clone() const
{
  return std::auto_ptr< SlabRegion>(new RandomRegion(*this));
}

void
RandomRegion::setPolys(const PolyMode::Value mode)
{
  myPolyMode = mode;
}

void
RandomRegion::setPolys(const PolyMode::Value mode,
    const PolygonNumbers & numbers)
{
  myPolyMode = mode;
  myPolyNumbers = numbers;
}

OrderedBasis::OrderedBasis(const std::vector< std::string> & species) :
    mySpecies(species)
{
}

bool
OrderedBasis::generateAtoms(const Voronoi & vd,
    const std::set< Voronoi::Vertex_handle> & vertices,
    std::vector< common::Atom> * const atoms) const
{
  std::set< Voronoi::Vertex_handle> toVisit(vertices);
  // Place all the atoms
  while(!toVisit.empty())
    placeAtoms(vd, nextBasisAtomIndex(vd, *toVisit.begin()), toVisit.begin(),
        &toVisit, atoms);
  return true;
}

UniquePtr< VoronoiSlabGenerator::SlabRegion::Basis>::Type
OrderedBasis::clone() const
{
  return UniquePtr< VoronoiSlabGenerator::SlabRegion::Basis>::Type(
      new OrderedBasis(*this));
}

void
OrderedBasis::placeAtoms(const Voronoi & vd, const size_t basisIdx,
    const std::set< Voronoi::Vertex_handle>::const_iterator & it,
    std::set< Voronoi::Vertex_handle> * const vertices,
    std::vector< common::Atom> * const atoms) const
{
  const Voronoi::Vertex_handle vtx = *it;
  vertices->erase(it);

  const Voronoi::Site_2 pt = vtx->point();
  arma::vec3 r;
  r(0) = CGAL::to_double(pt.x());
  r(1) = CGAL::to_double(pt.y());
  r(2) = 0.0;
  common::Atom atom(mySpecies[basisIdx]);
  atom.setPosition(r);
  atoms->push_back(atom);
  // Save the atom species with the dual face in the triangulation ds
  vtx->dual()->info().atomSpecies = atom.getSpecies();

  const Voronoi::Halfedge_around_vertex_circulator start =
      vd.incident_halfedges(vtx);
  Voronoi::Halfedge_around_vertex_circulator cl = start;
  do
  {
    const Voronoi::Vertex_handle neigh = cl->source();
    // Check if the neighbour is in our set of vertices
    const std::set< Voronoi::Vertex_handle>::const_iterator neighIt =
        vertices->find(neigh);
    if(neighIt != vertices->end())
      placeAtoms(vd, (basisIdx + 1) % mySpecies.size(), neighIt, vertices,
          atoms);
    ++cl;
    ++cl;
  }
  while(cl != start);
}

size_t
OrderedBasis::nextBasisAtomIndex(const Voronoi & vd,
    const Voronoi::Vertex_handle & vtx) const
{
  const Voronoi::Halfedge_around_vertex_circulator start =
      vd.incident_halfedges(vtx);
  Voronoi::Halfedge_around_vertex_circulator cl = start;
  size_t idx = 0;
  do
  {
    const Voronoi::Vertex_handle neigh = cl->source();
    if(!neigh->dual()->info().atomSpecies.empty())
    {
      // Find out if the neighbouring atom is one in our basis
      const std::vector< std::string>::const_iterator it = std::find(
          mySpecies.begin(), mySpecies.end(),
          neigh->dual()->info().atomSpecies);
      if(it != mySpecies.end())
      {
        idx = (std::distance(mySpecies.begin(), it) + 1) % mySpecies.size();
        break;
      }
    }

    ++cl;
    ++cl;
  }
  while(cl != start);
  return idx;
}

}
}

#endif // SPL_WITH_CGAL
