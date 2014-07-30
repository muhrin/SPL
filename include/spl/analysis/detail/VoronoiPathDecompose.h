/*
 * VoronoiPathDecompose.h
 *
 *  Created on: Apr 4, 2014
 *      Author: Martin Uhrin
 */

#ifndef VORONOI_PATH_DECOMPOSE_DETAIL_H
#define VORONOI_PATH_DECOMPOSE_DETAIL_H

// INCLUDES ///////////////////
#include <set>

#include <boost/foreach.hpp>

#include "spl/SSLibAssert.h"
#include "spl/analysis/VoronoiPathUtility.h"

// FORWARD DECLARATIONS ///////

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {
namespace detail {

struct NextHalfedgeType
{
  enum Value
  {
    IS_BOUNDARY, IS_NULL, IS_START
  };
};

template< typename VD>
  class GeneratePathVisitor : public std::binary_function<
      typename VD::Halfedge_handle, const typename NextHalfedgeType::Value, void>
  {
    typedef VD Voronoi;
    typedef VoronoiPathArrangement< VD> Arrangement;
    typedef typename Arrangement::Path Path;

  public:
    GeneratePathVisitor(Path * const path) :
        myPath(path)
    {
      SSLIB_ASSERT(myPath);
    }

    void
    operator()(typename Voronoi::Halfedge_handle he,
        const typename NextHalfedgeType::Value nextType)
    {
      if(nextType == NextHalfedgeType::IS_START)
        myPath->close(he);
      else
        myPath->push_back(he);
    }

  private:
    Path * const myPath;
  };

template< typename VD>
  class TrackLastHalfedge : public std::binary_function<
      typename VD::Halfedge_handle, const typename NextHalfedgeType::Value, void>
  {
    typedef VD Voronoi;
    typedef typename Voronoi::Halfedge_handle HalfedgeHandle;
  public:
    TrackLastHalfedge(HalfedgeHandle * const halfedge) :
        myHalfedge(halfedge)
    {
      SSLIB_ASSERT(halfedge);
    }

    void
    operator()(HalfedgeHandle he,
        const typename NextHalfedgeType::Value nextType)
    {
      *myHalfedge = he;
    }

  private:
    HalfedgeHandle * const myHalfedge;
  };

template< typename VD>
  size_t
  numZones(const typename VD::Vertex_handle & vtx, const VD & voronoi)
  {
    typedef VD Voronoi;
    typedef typename VoronoiLabel< Voronoi>::Type Label;

    size_t zones = 0;

    const typename Voronoi::Halfedge_around_vertex_circulator start =
        voronoi.incident_halfedges(vtx);
    typename Voronoi::Halfedge_around_vertex_circulator cl = start;
    do
    {
      const std::pair< Label, Label> & boundaryPair = getBoundaryPair< VD>(*cl);
      if(boundaryPair.first != boundaryPair.second)
        ++zones;
      ++cl;
    } while(cl != start);

    return zones;
  }

template< typename VD>
  typename VD::Halfedge_handle
  targetBoundaryHalfedge(const VD & voronoi,
      const typename VD::Halfedge_handle & he)
  {
    typedef VD Voronoi;
    typedef typename VoronoiLabel< VD>::Type Label;
    typedef typename Voronoi::Halfedge_handle HalfedgeHandle;

    SSLIB_ASSERT(isBoundary< VD>(*he));

    if(!he->has_target())
      return HalfedgeHandle();

    const std::pair< Label, Label> & boundaryPair = getBoundaryPair< VD>(*he);

    const typename Voronoi::Vertex_handle target = he->target();
    const typename Voronoi::Halfedge_around_vertex_circulator start =
        voronoi.incident_halfedges(target, he);
    typename Voronoi::Halfedge_around_vertex_circulator cl = start;

    const size_t zones = numZones(target, voronoi);
    if(zones == 4)
    {
      typename Voronoi::Halfedge_around_vertex_circulator cw = start;
      --cw;
      typename Voronoi::Halfedge_around_vertex_circulator ccw = start;
      ++ccw;
      if(getBoundaryPair< VD>(*cw) == getBoundaryPair< VD>(*ccw))
        return HalfedgeHandle();
      else if(getBoundaryPair< VD>(*cw->twin()) == boundaryPair)
        return cw->twin();
      else if(getBoundaryPair< VD>(*ccw->twin()) == boundaryPair)
        return ccw->twin();
      else
        return HalfedgeHandle();
    }
    else if(zones > 2)
      return HalfedgeHandle();

    HalfedgeHandle next;
    ++cl; // start corresponds to this halfedge, so move to the next one
    do
    {
      const HalfedgeHandle out = cl->twin();
      if(boundaryPair == getBoundaryPair< VD>(*out))
      {
        next = out;
        break;
      }
      ++cl;
    } while(cl != start);
    return next;
  }

template< typename VD, typename Visitor>
  void
  visitBoundaryHaledges(const VD & voronoi,
      const typename VD::Halfedge_handle & start, Visitor visitor)
  {
    typedef VD Voronoi;

    SSLIB_ASSERT(isBoundary< VD>(*start));

    typename Voronoi::Halfedge_handle next, he = start;
    typename NextHalfedgeType::Value nextType;
    do
    {
      next = targetBoundaryHalfedge< VD>(voronoi, he);
      if(next == typename Voronoi::Halfedge_handle())
        nextType = NextHalfedgeType::IS_NULL;
      else if(next == start)
        nextType = NextHalfedgeType::IS_START;
      else
        nextType = NextHalfedgeType::IS_BOUNDARY;

      visitor(he, nextType);

      he = next;
    } while(!(nextType == NextHalfedgeType::IS_NULL
        || nextType == NextHalfedgeType::IS_START));
  }

template< typename VD>
  void
  decomposeInternalPaths(const VD & voronoi,
      VoronoiPathArrangement< VD> * const arrangement)
  {
    typedef VD Voronoi;
    typedef typename Voronoi::Delaunay_graph Delaunay;
    typedef VoronoiPathArrangement< VD> Arrangement;
    typedef typename Arrangement::Path Path;

    const Delaunay & delaunay = voronoi.dual();

    std::set< typename Delaunay::Edge> visited;
    typename Voronoi::Halfedge_handle he;
    typename Delaunay::Edge dual;
    for(typename Voronoi::Edge_iterator it = voronoi.edges_begin(), end =
        voronoi.edges_end(); it != end; ++it)
    {
      dual = it->dual();
      if(spansBoundary< Delaunay>(dual) && visited.find(dual) == visited.end()
          && visited.find(delaunay.mirror_edge(dual)) == visited.end())
      {
        Path path(voronoi);

        // Trace the path out in one direction
        detail::visitBoundaryHaledges(voronoi, *it,
            detail::TrackLastHalfedge< VD>(&he));
        // Trace the path out from that endpoint in the opposite direction
        detail::visitBoundaryHaledges(voronoi, he->twin(),
            detail::GeneratePathVisitor< VD>(&path));

        BOOST_FOREACH(const typename Path::Edge & edge,
            boost::make_iterator_range(path.edgesBegin(), path.edgesEnd()))
          visited.insert(edge.delaunayEdge());

        arrangement->insertPath(path);
      }
    }
  }


}

template< typename VD>
  void
  decomposePaths(const VD & voronoi,
      VoronoiPathArrangement< VD> * const arrangement)
  {
    detail::decomposeInternalPaths(voronoi, arrangement);
  }

}
}

#endif /* VORONOI_PATH_DECOMPOSE_DETAIL_H */
