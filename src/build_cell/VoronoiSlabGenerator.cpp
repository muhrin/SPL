/*
 * VoronoiSlabGenerator.cpp
 *
 *  Created on: Jan 17, 2014
 *      Author: Martin Uhrin
 */

// INCLUEDES /////////////
#include "spl/build_cell/VoronoiSlabGenerator.h"

#ifdef SPL_WITH_CGAL

#include <boost/range/iterator_range.hpp>

#include <CGAL/centroid.h>

#include "spl/build_cell/PointSeparator.h"
#include "spl/common/Structure.h"
#include "spl/math/Matrix.h"
#include "spl/utility/Armadillo.h"

namespace spl {
namespace build_cell {

const int VoronoiSlabGenerator::MAX_ITERATIONS = 10000;

template< typename K>
  CGAL::Point_2< K>
  toCgalPoint2(const arma::vec3 & vec)
  {
    return CGAL::Point_2< K>(vec(0), vec(1));
  }

template< typename K>
  CGAL::Vector_2< K>
  toCgalVec2(const arma::vec3 & vec)
  {
    return CGAL::Vector_2< K>(vec(0), vec(1));
  }

template< typename K>
  arma::vec3
  toArmaVec3(const CGAL::Point_2< K> & vec)
  {
    arma::vec3 v;
    v(0) = CGAL::to_double(vec.x());
    v(1) = CGAL::to_double(vec.y());
    v(2) = 0.0;
    return v;
  }

VoronoiSlabGenerator::VoronoiSlabGenerator(const common::UnitCell & unitCell) :
    myUnitCell(unitCell)
{
}

GenerationOutcome
VoronoiSlabGenerator::generateStructure(common::StructurePtr & structure,
    const common::AtomSpeciesDatabase & speciesDb)
{
  structure.reset(new common::Structure());
  if(myUnitCell)
    structure->setUnitCell(*myUnitCell);

  BOOST_FOREACH(const Slab & slab, mySlabs)
    slab.generateSlab(structure.get());

  return GenerationOutcome::success();
}

GenerationOutcome
VoronoiSlabGenerator::generateStructure(common::StructurePtr & structure,
    const common::AtomSpeciesDatabase & speciesDb,
    const GenerationSettings & info)
{
  return generateStructure(structure, speciesDb);
}

void
VoronoiSlabGenerator::addSlab(const Slab & slab)
{
  mySlabs.push_back(slab);
}

VoronoiSlabGenerator::Slab::Slab() :
    myTransform(arma::eye(4, 4)), mySaveTriangulation(false)
{
}

VoronoiSlabGenerator::Slab::Slab(const arma::mat44 & transform) :
    myTransform(transform), mySaveTriangulation(false)
{
}

class VoronoiSlabGenerator::Slab::SlabData
{
public:
  typedef std::multimap< Delaunay::Vertex_handle,
      std::pair< K::Vector_2, Delaunay::Vertex_handle> > PeriodicImages;

  SlabData(const common::Structure & structure);

  void
  generatePeriodicImages();
  void
  updatePeriodicImages();
  void
  wrapFundamentalVertices();

  Delaunay dg;
  std::vector< Delaunay::Vertex_handle> vertices;
  PeriodicImages periodicImages;
  RegionTickets tickets;
  const common::DistanceCalculator & distCalc;
  const common::UnitCell * const unitCell;
};

VoronoiSlabGenerator::Slab::SlabData::SlabData(
    const common::Structure & structure) :
    distCalc(structure.getDistanceCalculator()), unitCell(
        structure.getUnitCell())
{
}

void
VoronoiSlabGenerator::Slab::SlabData::generatePeriodicImages()
{
  K::Vector_2 a, dr;
  for(Delaunay::Vertex_iterator it = dg.vertices_begin(), end =
      dg.vertices_end(); it != end; ++it)
    vertices.push_back(it);
  if(unitCell)
  {
    BOOST_FOREACH(const Delaunay::Vertex_handle vtx, vertices)
    {
      for(int i = -1; i <= 1; ++i)
      {
        a = toCgalVec2< K>(static_cast< double>(i) * unitCell->getAVec());
        for(int j = -1; j <= 1; ++j)
        {
          if(!(i == 0 && j == 0))
          {
            dr = a
                + toCgalVec2< K>(static_cast< double>(j) * unitCell->getBVec());
            const Delaunay::Vertex_handle image = dg.insert(vtx->point() + dr);
            image->info() = vtx->info();
            image->info().isImage = true;
            periodicImages.insert(
                PeriodicImages::value_type(vtx, std::make_pair(dr, image)));
          }
        }
      }
    }
  }
}

void
VoronoiSlabGenerator::Slab::SlabData::updatePeriodicImages()
{
  BOOST_FOREACH(PeriodicImages::const_reference image, periodicImages)
    dg.move(image.second.second, image.first->point() + image.second.first);
}

void
VoronoiSlabGenerator::Slab::SlabData::wrapFundamentalVertices()
{
  if(!unitCell)
    return;

  arma::vec3 pt;
  BOOST_FOREACH(const Delaunay::Vertex_handle & vtx, vertices)
  {
    pt = toArmaVec3(vtx->point());
    unitCell->wrapVecInplace(pt);
    dg.move_if_no_collision(vtx, toCgalPoint2< K>(pt));
  }
}

bool
VoronoiSlabGenerator::Slab::generateSlab(
    common::Structure * const structure) const
{
  SSLIB_ASSERT(structure);

  SlabData slabData(*structure);

  createTriangulation(&slabData);
  refineTriangulation(&slabData);
  generateAtoms(slabData, structure);

  if(mySaveTriangulation)
  {
    std::string trianFile = structure->getName();
    if(trianFile.empty())
      trianFile = "trian";
    trianFile += ".dat";
    std::ofstream trianOut(trianFile.c_str());
    if(trianOut.is_open())
    {
      trianOut << slabData.dg;
      trianOut.close();
    }
  }

  return true;
}

void
VoronoiSlabGenerator::Slab::addRegion(UniquePtr< SlabRegion>::Type & region)
{
  myRegions.push_back(region);
}

void
VoronoiSlabGenerator::Slab::setSaveTriangulation(const bool save)
{
  mySaveTriangulation = save;
}

void
VoronoiSlabGenerator::Slab::createTriangulation(SlabData * const slabData) const
{
  BOOST_FOREACH(const VoronoiSlabGenerator::SlabRegion & region, myRegions)
  {
    RegionTicket ticket = region.generateSites(&slabData->dg);
    if(ticket.valid())
      slabData->tickets[&region] = ticket;
  }
  slabData->generatePeriodicImages();
}

bool
VoronoiSlabGenerator::Slab::refineTriangulation(SlabData * const slabData) const
{
  typedef std::set< Delaunay::Vertex_handle> RegionSites;
  typedef std::map< const SlabRegion *, RegionSites> UpdateRegions;

  std::vector< Delaunay::Vertex_handle> toRemove;
  UpdateRegions updateRegions;
  bool allRegionsGood;

  for(int i = 0; i < MAX_ITERATIONS; ++i)
  {
    // Fix up the triangulation
    separatePoints(slabData);
    equaliseEdges(slabData);

    // Find out which region each site belongs to and delete any that have
    // strayed out
    BOOST_FOREACH(const VoronoiSlabGenerator::SlabRegion & region, myRegions)
    {
      const bool haveTicket = slabData->tickets.find(&region)
          != slabData->tickets.end();
      toRemove.clear();
      BOOST_FOREACH(Delaunay::Vertex_handle & vtx, slabData->vertices)
      {
        if(vtx->info().generatedBy == &region)
        {
          /*if(!region.withinBoundary(it->point()))
            toRemove.push_back(it);
          else*/
          if(haveTicket)
            updateRegions[&region].insert(vtx);
        }
      }
      BOOST_FOREACH(Delaunay::Vertex_handle & vtx, toRemove)
        slabData->dg.remove(vtx);
    }

    // Check each site that provided a ticket to see if it's happy with the
    // current state of the region
    allRegionsGood = true;
    BOOST_FOREACH(UpdateRegions::reference update, updateRegions)
    {
      if(!update.first->good(slabData->tickets.find(update.first)->second,
          update.second, slabData->dg))
      {
        allRegionsGood = false;
        break;
      }
    }
    if(allRegionsGood)
      break;

    // Regions need more refinement
    BOOST_FOREACH(UpdateRegions::reference update, updateRegions)
    {
      update.first->refine(slabData->tickets.find(update.first)->second,
          update.second, &slabData->dg);
      update.second.clear();
    }
    slabData->updatePeriodicImages();
  }

  return allRegionsGood;
}

bool
VoronoiSlabGenerator::Slab::separatePoints(SlabData * const slabData) const
{
  const size_t numVertices = slabData->vertices.size();
  SeparationData sepData(numVertices, slabData->distCalc);

  sepData.separations.fill(0.0);
  sepData.points.row(2).fill(0.0); // Set all z values to 0
  size_t idx = 0;
  BOOST_FOREACH(const Delaunay::Vertex_handle vtx, slabData->vertices)
  {
    sepData.points.col(idx) = toArmaVec3(vtx->point());
    if(vtx->info().fixed)
      sepData.fixedPoints.insert(idx);

    if(vtx->info().minsep)
    {
      for(size_t i = 0; i < numVertices; ++i)
      {
        double sep = std::max(sepData.separations(idx, i), *vtx->info().minsep);
        sepData.separations(i, idx) = sepData.separations(idx, i) = sep;
      }
    }
    ++idx;
  }

  if(slabData->unitCell)
    slabData->unitCell->wrapVecsInplace(sepData.points);

  static const PointSeparator POINT_SEPARATOR(1000, 0.1);
  const bool result = POINT_SEPARATOR.separatePoints(&sepData);

  // Copy back the separated positions, even if it didn't get the positions
  // all the way to the tolerance
  idx = 0;
  BOOST_FOREACH(const Delaunay::Vertex_handle vtx, slabData->vertices)
  {
    slabData->dg.move_if_no_collision(vtx,
        Delaunay::Point(sepData.points(0, idx), sepData.points(1, idx)));
    ++idx;
  }

  slabData->updatePeriodicImages();

  return result;
}

void
VoronoiSlabGenerator::Slab::equaliseEdges(SlabData * const slabData) const
{
  typedef std::map< Delaunay::Vertex_handle, K::Vector_2> Forces;

  static const double FORCE_FACTOR = 0.5;

  Forces forces;
  double maxForceSq;

  do
  {
    forces.clear();

    BOOST_FOREACH(const Delaunay::Vertex_handle vtx, slabData->vertices)
    {
      if(vtx->info().fixed || vtx->degree() < 3
          || detail::isBoundaryVertex(slabData->dg, vtx))
        continue;

      const K::Point_2 p0 = vtx->point();
      K::Vector_2 f(0.0, 0.0), r;

      const Delaunay::Vertex_circulator start = vtx->incident_vertices();
      Delaunay::Vertex_circulator cl = start;
      if(start.is_empty())
        continue;

      // Build up the list of points that make up the polygon surrounding this vertex
      std::vector< K::Point_2> poly;
      do
      {
        poly.push_back(cl->point());
        ++cl;
      }
      while(cl != start);

      forces[vtx] = FORCE_FACTOR
          * (CGAL::centroid(poly.begin(), poly.end()) - p0);
    }
    // Now apply all the forces
    maxForceSq = 0.0;
    BOOST_FOREACH(Forces::const_reference f, forces)
    {
      maxForceSq = std::max(maxForceSq,
          CGAL::to_double(f.second.squared_length()));
      slabData->dg.move_if_no_collision(f.first, f.first->point() + f.second);
    }
    if(slabData->unitCell)
    {
      slabData->wrapFundamentalVertices();
      slabData->updatePeriodicImages();
    }
  }
  while(maxForceSq > 0.0001);
}

void
VoronoiSlabGenerator::Slab::generateAtoms(const SlabData & slabData,
    common::Structure * const structure) const
{
  typedef std::set< Voronoi::Vertex_handle> VerticesSet;
  typedef std::list< Voronoi::Vertex_handle> VerticesList;

  const Voronoi vd(slabData.dg);

  VerticesList allVertices;
  for(Voronoi::Vertex_iterator it = vd.vertices_begin(), end =
      vd.vertices_end(); it != end; ++it)
  {
    // TODO: Only use faces where at least one vertex is in the fundamental domain
    // if(slabData.unitCell && !slabData.unitCell->isInside(convert(it))
    //   continue;
    allVertices.push_back(it);
  }

  std::vector< common::Atom> atoms;
  BOOST_FOREACH(const VoronoiSlabGenerator::SlabRegion & region, myRegions)
  {
    VerticesSet regionVertices;
    for(VerticesList::iterator it = allVertices.begin(), end = allVertices.end();
        it != end; /*increment in body*/)
    {
      VerticesList::iterator next = it;
      ++next;
      Voronoi::Vertex_handle & vtx = *it;
      if(true)
      {
        if(region.withinBoundary(vtx->point()))
        {
          regionVertices.insert(vtx);
          allVertices.erase(it);
        }
      }
      // Move on the iterator
      it = next;
    }
    region.generateAtoms(vd, regionVertices, &atoms);
  }

  BOOST_FOREACH(const common::Atom & atom, atoms)
    structure->newAtom(atom).setPosition(
        math::transformCopy(atom.getPosition(), myTransform));
}

VoronoiSlabGenerator::SlabRegion::SlabRegion(
    const std::vector< arma::vec2> & boundary, UniquePtr< Basis>::Type & basis) :
    myBasis(basis)
{
  BOOST_FOREACH(const arma::vec2 & r, boundary)
    myBoundary.push_back(utility::toCgalPoint< K>(r));
  SSLIB_ASSERT(!myBoundary.is_empty());
  SSLIB_ASSERT(myBoundary.is_simple());
}

VoronoiSlabGenerator::SlabRegion::SlabRegion(const SlabRegion & toCopy) :
    myBoundary(toCopy.myBoundary), myBasis(toCopy.myBasis->clone())
{
}

const VoronoiSlabGenerator::SlabRegion::Boundary &
VoronoiSlabGenerator::SlabRegion::getBoundary() const
{
  return myBoundary;
}

bool
VoronoiSlabGenerator::SlabRegion::withinBoundary(const arma::vec2 & r) const
{
  return withinBoundary(utility::toCgalPoint< K>(r));
}
bool
VoronoiSlabGenerator::SlabRegion::withinBoundary(
    const Delaunay::Point & r) const
{
  return getBoundary().bounded_side(r) != CGAL::ON_UNBOUNDED_SIDE;
}

void
VoronoiSlabGenerator::SlabRegion::generateAtoms(const Voronoi & vd,
    const std::set< Voronoi::Vertex_handle> & vertices,
    std::vector< common::Atom> * const atoms) const
{
  myBasis->generateAtoms(vd, vertices, atoms);
}

VoronoiSlabGenerator::SlabRegion *
new_clone(const VoronoiSlabGenerator::SlabRegion & region)
{
  return region.clone().release();
}

} // namespace build_cell
} // namespace spl

#endif // SPL_WITH_CGAL
