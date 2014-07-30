/*
 * VoronoiSlabRegions.h
 *
 *  Created on: Jan 17, 2014
 *      Author: Martin Uhrin
 */

#ifndef VORONOI_SLAB_REGIONS_H
#define VORONOI_SLAB_REGIONS_H

// INCLUDES /////////////////////////////////
#include "spl/SSLib.h"

#ifdef SPL_USE_CGAL

#include <map>
#include <vector>

#include <boost/optional.hpp>

#include "spl/build_cell/VoronoiSlabGenerator.h"

#include "spl/utility/DataPool.h"
#include "spl/utility/Range.h"

// FORWARD DECLARES //////////////////////////

namespace spl {
namespace math {
class Line2;
}
namespace build_cell {

class LatticeRegion : public VoronoiSlabGenerator::SlabRegion
{
  typedef VoronoiSlabGenerator::SlabRegion::K K;
public:
  struct ConstructionInfo
  {
    std::vector< arma::vec2> boundary;
    arma::vec2 vecA;
    arma::vec2 vecB;
    boost::optional< arma::vec2> startPoint;
    boost::optional< bool> fixPoints;
  };

  LatticeRegion(const ConstructionInfo & info, UniquePtr< Basis>::Type & basis);
  LatticeRegion(const LatticeRegion & toCopy);

  virtual Ticket
  generateSites(Delaunay * const dg) const;
  virtual std::auto_ptr< SlabRegion>
  clone() const;

private:
  typedef utility::Range< int> LatticeMultiples;

  LatticeMultiples
  getLatticeMultiples(const arma::vec2 & x0, const arma::vec2 & latticeVecP,
      const arma::vec2 & latticeVecQ) const;
  int
  getLatticeVecMultiple(const arma::vec2 & x0, const arma::vec2 & latticeVec,
      const math::Line2 & latticeLine,
      const math::Line2 & intersectionLine) const;
  void
  generateLine(const arma::vec2 & r0, const arma::vec2 & dr,
      const LatticeMultiples & range, const CGAL::Bbox_2 & box,
      Delaunay * const dg) const;
  bool
  inBBox(const arma::vec2 & r, const CGAL::Bbox_2 & bbox) const;

  arma::vec2 myLattice[2];
  boost::optional< arma::vec2> myStartPoint;
  bool myFixPoints;
};

class RandomRegion : public VoronoiSlabGenerator::SlabRegion
{
  typedef VoronoiSlabGenerator::SlabRegion::K K;
public:
  typedef std::map< int, int> PolygonNumbers;
  struct PolyMode
  {
    enum Value
    {
      FREE, STOCHASTIC, FIXED
    };
  };

  RandomRegion(const std::vector< arma::vec2> & boundary, const int numPoints,
      const double minsep, UniquePtr< Basis>::Type & basis);

  virtual Ticket
  generateSites(Delaunay * const dg) const;
  virtual bool
  refine(const Ticket & ticket,
      const std::set< Delaunay::Vertex_handle> & vertices,
      Delaunay * const dg) const;
  virtual bool
  good(const Ticket & ticket,
      const std::set< Delaunay::Vertex_handle> & vertices,
      const Delaunay & dg) const;
  virtual std::auto_ptr< SlabRegion>
  clone() const;

  void
  setPolys(const PolyMode::Value mode);
  void
  setPolys(const PolyMode::Value mode, const PolygonNumbers & numbers);

private:
  void
  generateSites(const size_t num, Delaunay * const dg) const;
  PolygonNumbers
  measurePolyNumbers(const Delaunay & dg,
      const std::set< Delaunay::Vertex_handle> & vertices) const;
  PolygonNumbers
  getPolyDiffs(const Delaunay & dg,
      const std::set< Delaunay::Vertex_handle> & vertices,
      const PolygonNumbers & target) const;

  const int myNumPoints;
  const double myMinsep;
  PolyMode::Value myPolyMode;
  PolygonNumbers myPolyNumbers;
  mutable utility::DataPool< PolygonNumbers> myPolyPool;
};

class OrderedBasis : public VoronoiSlabGenerator::SlabRegion::Basis
{
  typedef VoronoiSlabGenerator::SlabRegion::Voronoi Voronoi;
public:
  explicit
  OrderedBasis(const std::vector< std::string> & species);

  virtual bool
  generateAtoms(const Voronoi & vd,
      const std::set< Voronoi::Vertex_handle> & vertices,
      std::vector< common::Atom> * const atoms) const;

  virtual UniquePtr< Basis>::Type
  clone() const;
private:
  void
  placeAtoms(const Voronoi & vd, const size_t basisIdx,
      const std::set< Voronoi::Vertex_handle>::const_iterator & it,
      std::set< Voronoi::Vertex_handle> * const vertices,
      std::vector< common::Atom> * const atoms) const;
  size_t
  nextBasisAtomIndex(const Voronoi & vd,
      const Voronoi::Vertex_handle & vtx) const;

  std::vector< std::string> mySpecies;
};

}
}

#endif // SPL_USE_CGAL
#endif /* VORONOI_SLAB_REGIONS_H */
