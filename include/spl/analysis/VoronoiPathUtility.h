/*
 * VoronoiPathUtility.h
 *
 *  Created on: Mar 18, 2014
 *      Author: Martin Uhrin
 */

#ifndef VORONOI_PATH_UTILITY_H
#define VORONOI_PATH_UTILITY_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_USE_CGAL

#include <boost/foreach.hpp>
#include <boost/range/iterator_range.hpp>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Homogeneous.h>
#include <CGAL/intersections.h>
#include <CGAL/Polytope_distance_d.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polytope_distance_d_traits_2.h>

#include "spl/analysis/MapArrangementTraits.h"
#include "spl/utility/Range.h"

// FORWARD DECLARATIONS ///////

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {
namespace detail {

template< typename K>
  CGAL::Point_2< K>
  interval(const typename K::FT & lambda, const CGAL::Point_2< K> & x0,
      const CGAL::Point_2< K> & x1)
  {
    return x0 + lambda * (x1 - x0);
  }

template< typename K>
  CGAL::Point_2< K>
  closestPoint(const CGAL::Segment_2< K> & seg,
      const CGAL::Polygon_2< K> & poly)
  {
    typedef CGAL::Point_2< K> Point;
    typedef typename K::FT FT;

    typedef CGAL::MP_Float ET;
    typedef CGAL::Polytope_distance_d_traits_2< K, ET, typename K::RT> DistTraits;
    typedef CGAL::Polytope_distance_d< DistTraits> PolyDist;

    const Point line[] =
      { seg.source(), seg.target() };
    PolyDist dist(poly.vertices_begin(), poly.vertices_end(), line, line + 2);

    typename PolyDist::Coordinate_iterator it =
        dist.realizing_point_p_coordinates_begin();

    const FT x(CGAL::to_double(*it));
    const FT y(CGAL::to_double(*++it));
    const FT denom(CGAL::to_double(*++it));

    return Point(x / denom, y / denom);
  }

}

// Convenience struct-typedef for getting the Delaunay vertex info label from a
// Voronoi diagram type
template< class VD>
  struct VoronoiLabel
  {
    typedef typename VD::Delaunay_graph::Triangulation_data_structure::Vertex::Info Type;
  };

// Convenience struct-typedef for getting the Delaunay vertex info label from a
// Delaunay graph type
template< class DG>
  struct DelaunayLabel
  {
    typedef typename DG::Triangulation_data_structure::Vertex::Info Type;
  };

template< typename Label>
  struct BoundaryPair
  {
    typedef spl::utility::OrderedPair< Label> Type;
  };

template< class DG>
  typename BoundaryPair< typename DelaunayLabel< DG>::Type>::Type
  getSpanningPair(const typename DG::Edge edge);

// Is this Voronoi halfedge a boundary between regions of different labels
template< class VD>
  bool
  isBoundary(const typename VD::Halfedge & he);

// Does this Delaunay edge span the boundary between regions of different
// labels
template< class DG>
  bool
  spansBoundary(const typename DG::Edge & edge);

template< typename VD>
  class VoronoiPathArrangement;

template< typename VD>
  CGAL::Polygon_2< typename VD::Delaunay_geom_traits>
  delaunayDomain(const typename VD::Vertex_handle & vtx, const VD & voronoi);

template< typename MapTraits, typename VD>
  typename MapTraits::Arrangement
  toMap(const VoronoiPathArrangement< VD> & pathArrangement);

}
}

#include "spl/analysis/detail/VoronoiPathUtility.h"

#endif /* SPL_USE_CGAL */
#endif /* VORONOI_PATH_UTILITY_H */
