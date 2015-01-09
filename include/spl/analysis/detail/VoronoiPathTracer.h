/*
 * VoronoiPathTracer.h
 *
 *  Created on: Feb 18, 2014
 *      Author: Martin Uhrin
 */

#ifndef VORONOI_PATH_TRACER_DETAIL_H
#define VORONOI_PATH_TRACER_DETAIL_H

// INCLUDES ///////////////////

#include <set>
#include <iostream>
#include <map>

#include <boost/foreach.hpp>

#include <armadillo>

#include <CGAL/Arrangement_2.h>
#include <CGAL/Arr_extended_dcel.h>
#include <CGAL/Arr_segment_traits_2.h>

#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Voronoi_diagram_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_policies_2.h>
#include <CGAL/Linear_algebraCd.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Voronoi_diagram_2.h>

#include <CGAL/linear_least_squares_fitting_2.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Gmpfr.h>
#include <CGAL/Polytope_distance_d.h>
#include <CGAL/Polytope_distance_d_traits_2.h>

#include "spl/SSLibAssert.h"
#include "spl/analysis/MapArrangementTraits.h"
#include "spl/utility/Range.h"
#include "spl/utility/StableComparison.h"
#include "spl/utility/TransformFunctions.h"
#include "spl/analysis/VoronoiPathArrangement.h"
#include "spl/analysis/VoronoiPathDecompose.h"
#include "spl/analysis/VoronoiPathUtility.h"

// FORWARD DECLARATIONS ///////

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {
namespace detail {

template< typename MapTraits>
  class VoronoiPathTracer
  {
    typedef typename MapTraits::Label Label;
    typedef typename MapTraits::Arrangement Map;

    typedef CGAL::Exact_predicates_inexact_constructions_kernel K; // The kernel type
    typedef typename K::FT FT; // The field type

    // typedefs for defining the adaptor
    typedef CGAL::Triangulation_vertex_base_with_info_2< Label, K> Vb;
    typedef CGAL::Triangulation_data_structure_2< Vb> Tds;

  public:
    typedef typename K::Line_2 Line;
    typedef typename K::Point_2 Point;
    typedef CGAL::Polygon_2< K> Polygon;
    typedef typename K::Segment_2 Segment;
    typedef typename K::Vector_2 Vector;
    typedef CGAL::Delaunay_triangulation_2< K, Tds> Delaunay;

  private:
    typedef CGAL::Delaunay_triangulation_adaptation_traits_2< Delaunay> AT;
    typedef CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<
        Delaunay> AP;

  public:
    typedef CGAL::Voronoi_diagram_2< Delaunay, AT, AP> Voronoi;
    typedef VoronoiPathArrangement< Voronoi> PathArrangement;

  public:
    PathArrangement
    generatePaths(const Voronoi & voronoi) const;

  private:
    typedef std::pair< size_t, size_t> Subpath;
    typedef std::vector< size_t> PossiblePath;

    struct NextHalfedgeType
    {
      enum Value
      {
        IS_BOUNDARY, IS_NULL, IS_START
      };
    };
  public:
    typedef VoronoiPath< Voronoi> Path;

  private:
    class DirectionChecker;
    struct PathInfo;
    struct MeetingInfo;

    static Point
    midpoint(const typename Delaunay::Edge & edge);
    Segment
    segment(const typename Delaunay::Edge & edge) const;

    std::vector< ptrdiff_t>
    findStraightPathsInternal(const Path & path,
        const PathArrangement & arr) const;
    bool
    isStraight(const Path & path, size_t i, size_t k) const;
    bool
    isStraight(const typename Delaunay::Edge & startEdge,
        const typename Delaunay::Edge & endEdge, const Path & path,
        const size_t pathEdgeBegin, const size_t pathEdgeLast,
        DirectionChecker * const dirs) const;

    PossiblePath
    findOptimumPath(const Path & path,
        const std::vector< ptrdiff_t> & longest) const;
    double
    penalty(const Path & fullPath, const Subpath & subpath) const;

    void
    adjustVertices(Path * const path) const;
    void
    optimiseBoundaryVertices(PathArrangement * const arr) const;
    void
    optimiseMeetingVertices(PathArrangement * const arr) const;
    void
    smooth(Path * const path) const;

    Point
    joinLines(const typename CGAL::Linear_algebraCd< FT>::Matrix & quad,
        const Polygon & boundary) const;
    Polygon
    surroundingBoundary(const typename Delaunay::Edge & edge,
        const Voronoi & voronoi) const;
    FT
    quadDist(const Point & p,
        const typename CGAL::Linear_algebraCd< FT>::Matrix & Q) const;

    ptrdiff_t
    pEnd(const ptrdiff_t i, const Path & p) const
    {
      return p.isClosed() ? i + p.numVertices() : p.numVertices();
    }

    size_t
    mod(const ptrdiff_t i, const ptrdiff_t n) const
    {
      return i < 0 ? n - (-i % n) : i % n;
    }

    // Used when we need an index that can work for both closed and open paths
    // where for closed paths the index is the standard wrapping (mod)
    // for open paths the index is i => i for i=0:n and i => n + 1 for i < 0
    // This allows the user to use the 'before the first' and 'after the last'
    // indices transparently
    ptrdiff_t
    longestIdx(const ptrdiff_t i, const Path & p) const
    {
      const ptrdiff_t n = p.numVertices();
      if(p.isClosed())
        return mod(i, n);
      else
      {
        SSLIB_ASSERT(i <= n);
        return i < 0 ? n + 1 : i;
      }
    }
  };

template< typename Voronoi>
  CGAL::Polygon_2< typename Voronoi::Delaunay_graph::Geom_traits>
  surroundingPolygon(const typename Voronoi::Vertex & vtx)
  {
    CGAL::Polygon_2< typename Voronoi::Delaunay_graph::Geom_traits> poly;
    const typename Voronoi::Halfedge_around_vertex_circulator start =
        vtx.incident_halfedges();
    typename Voronoi::Halfedge_around_vertex_circulator cl = start;
    do
    {
      poly.push_back(cl->face()->dual()->point());
      ++cl;
    } while(cl != start);
    return poly;
  }

template< typename MapTraits>
  class VoronoiPathTracer< MapTraits>::DirectionChecker
  {
    struct Direction
    {
      enum Value
      {
        UP, LEFT, DOWN, RIGHT
      };
    };
  public:
    void
    update(const Vector & dr)
    {
      namespace stable = spl::utility::stable;

      if(numDirections() == 4)
        return;

      if(stable::gt(dr.x(), 0.0))
        myDirections.insert(Direction::RIGHT);
      else if(stable::lt(dr.x(), 0.0))
        myDirections.insert(Direction::LEFT);

      if(stable::gt(dr.y(), 0.0))
        myDirections.insert(Direction::UP);
      else if(stable::lt(dr.y(), 0.0))
        myDirections.insert(Direction::DOWN);
    }
    size_t
    numDirections() const
    {
      return myDirections.size();
    }
  private:
    std::set< typename Direction::Value> myDirections;
  };

template< typename MapTraits>
  typename VoronoiPathTracer< MapTraits>::Point
  VoronoiPathTracer< MapTraits>::midpoint(const typename Delaunay::Edge & edge)
  {
    return CGAL::midpoint(edge.first->vertex((edge.second + 1) % 3)->point(),
        edge.first->vertex((edge.second + 2) % 3)->point());
  }

template< typename MapTraits>
  typename VoronoiPathTracer< MapTraits>::Segment
  VoronoiPathTracer< MapTraits>::segment(
      const typename Delaunay::Edge & edge) const
  {
    return Segment(edge.first->vertex((edge.second + 1) % 3)->point(),
        edge.first->vertex((edge.second + 2) % 3)->point());
  }

template< typename MapTraits>
  struct VoronoiPathTracer< MapTraits>::PathInfo
  {
    std::vector< Line> fitLines;
    const Path * orig;
    Path adjusted;
  };

template< typename MapTraits>
  typename VoronoiPathTracer< MapTraits>::PathArrangement
  VoronoiPathTracer< MapTraits>::generatePaths(const Voronoi & voronoi) const
  {
    typename VoronoiPathTracer< MapTraits>::PathArrangement arr(voronoi);
    decomposePaths(voronoi, &arr);

    BOOST_FOREACH(Path & path,
        boost::make_iterator_range(arr.pathsBegin(), arr.pathsEnd()))
    {
      const std::vector< ptrdiff_t> & longest = findStraightPathsInternal(path,
          arr);
      const PossiblePath & optimal = findOptimumPath(path, longest);
      path.setOptimalPath(optimal);

      adjustVertices(&path);
    }

    optimiseBoundaryVertices(&arr);
    optimiseMeetingVertices(&arr);

    BOOST_FOREACH(Path & path,
        boost::make_iterator_range(arr.pathsBegin(), arr.pathsEnd()))
      smooth(&path);

    return arr;
  }

template< typename MapTraits>
  std::vector< ptrdiff_t>
  VoronoiPathTracer< MapTraits>::findStraightPathsInternal(const Path & path,
      const PathArrangement & arr) const
  {
    typedef typename Path::Edge Edge;
    typedef typename PathArrangement::MeetingVerticesConst MeetingVertices;

    const ptrdiff_t n = static_cast< ptrdiff_t>(path.numVertices());

    arma::Mat< int> straight(n + 2, n + 2);
    straight.zeros();
    for(ptrdiff_t i = 0; i < n; ++i) // Start vertex
    {
      for(ptrdiff_t k = i + 1; k < pEnd(i, path); ++k) // End vertex
        straight(longestIdx(i, path), longestIdx(k, path)) = isStraight(path, i,
            path.wrapIndex(k));
    }

    if(!path.isClosed())
    {
      bool pathStraight;
      const MeetingVertices & meeting = arr.getMeetingVertices();
      std::pair< typename MeetingVertices::const_iterator,
          typename MeetingVertices::const_iterator> meetingRange;

      // Vertex before the start meeting vertex
      if(!path.vertexFront().isBoundary())
      {
        // Paths of length 1-2 are always possible
        for(ptrdiff_t i = 0; i < std::min(static_cast< ptrdiff_t>(2), n); ++i)
          straight(longestIdx(-1, path), longestIdx(i, path)) = true;

        meetingRange = meeting.equal_range(path.vertex(0).voronoiVertex());

        for(ptrdiff_t i = 2; i < n; ++i) // End vertex
        {
          const typename Path::Edge & e1 = path.edge(i - 1);

          pathStraight = true;
          BOOST_FOREACH(typename PathArrangement::MeetingVerticesConst::const_reference vtx,
              meetingRange)
          {
            const Path * const path2 = vtx.second.path();
            const typename Path::Edge & e2 =
                vtx.second.idx() == 0 ? path2->edgeFront() : path2->edgeBack();

            DirectionChecker dirs;
            // Put in the start and end vectors
            dirs.update(
                path.vertex(e1.target()).point()
                    - path.vertex(e1.source()).point());
            dirs.update(
                path2->vertex(e2.target()).point()
                    - path2->vertex(e2.source()).point());

            if(!isStraight(e1.delaunayEdge(), e2.delaunayEdge(), path, 0, i - 2,
                &dirs))
            {
              pathStraight = false;
              break;
            }
          }
          straight(longestIdx(-1, path), longestIdx(i, path)) = pathStraight;
        }
      }

      // Vertex after the end meeting vertex
      if(n > 1 && !path.vertexBack().isBoundary())
      {
        // Paths of length 1-2 are always possible
        for(ptrdiff_t i = n - 1;
            i >= std::max(n - 2, static_cast< ptrdiff_t>(0)); --i)
          straight(longestIdx(i, path), longestIdx(n, path)) = true;

        meetingRange = meeting.equal_range(path.vertex(n - 1).voronoiVertex());

        for(ptrdiff_t i = n - 3; i >= 0; --i) // End vertex
        {
          const Edge & e1 = path.edge(i);

          pathStraight = true;
          BOOST_FOREACH(typename MeetingVertices::const_reference vtx,
              meetingRange)
          {
            const Path * const path2 = vtx.second.path();
            const Edge & e2 =
                vtx.second.idx() == 0 ? path2->edgeFront() : path2->edgeBack();

            DirectionChecker dirs;
            // Put in the start and end vectors
            dirs.update(
                path.vertex(e1.target()).point()
                    - path.vertex(e1.source()).point());
            dirs.update(
                path2->vertex(e2.target()).point()
                    - path2->vertex(e2.source()).point());

            if(!isStraight(e1.delaunayEdge(), e2.delaunayEdge(), path, i + 1,
                path.numEdges() - 1, &dirs))
            {
              pathStraight = false;
              break;
            }
          }
          straight(longestIdx(i, path), longestIdx(n, path)) = pathStraight;
        }
      }

      if(n > 1 && !path.vertexFront().isBoundary()
          && !path.vertexBack().isBoundary())
      {
        meetingRange = meeting.equal_range(path.vertexFront().voronoiVertex());
        std::pair< typename MeetingVertices::const_iterator,
            typename MeetingVertices::const_iterator> meetingRange2 =
            meeting.equal_range(path.vertexBack().voronoiVertex());
        pathStraight = true;
        BOOST_FOREACH(typename MeetingVertices::const_reference vtx1,
            meetingRange)
        {
          const Path * const path1 = vtx1.second.path();
          const Edge & e1 =
              vtx1.second.idx() == 0 ? path1->edgeFront() : path1->edgeBack();

          BOOST_FOREACH(typename MeetingVertices::const_reference vtx2,
              meetingRange2)
          {
            const Path * const path2 = vtx2.second.path();
            const Edge & e2 =
                vtx2.second.idx() == 0 ? path2->edgeFront() : path2->edgeBack();

            DirectionChecker dirs;
            // Put in the start and end vectors
            dirs.update(
                path1->vertex(e1.target()).point()
                    - path1->vertex(e1.source()).point());
            dirs.update(
                path2->vertex(e2.target()).point()
                    - path2->vertex(e2.source()).point());

            if(!isStraight(e1.delaunayEdge(), e2.delaunayEdge(), path, 0, n - 2,
                &dirs))
            {
              pathStraight = false;
              break;
            }
          }
          if(!pathStraight)
            break;
        }
        straight(longestIdx(-1, path), longestIdx(n, path)) = pathStraight;
      }
    }

    const ptrdiff_t s0 = path.isClosed() ? 0 : -1, s1 =
        path.isClosed() ? n : n + 1;
    std::vector< ptrdiff_t> longest(n + 2);
    for(ptrdiff_t i = s0; i < s1; ++i) // Start vertex
    {
      const ptrdiff_t dMax = path.isClosed() ? n : n - i + 1;
      for(ptrdiff_t d = 1; d < dMax; ++d)
      {
        const ptrdiff_t k = path.isClosed() ? path.wrapIndex(i + d) : i + d; // End vertex
        bool allStraight = true;
        for(ptrdiff_t l = i; l < k; ++l) // Start
        {
          for(ptrdiff_t m = l + 1; m <= k; ++m) // End
          {
            if(!straight(longestIdx(l, path), longestIdx(m, path)))
            {
              allStraight = false;
              break;
            }
          }
          if(!allStraight)
            break;
        }
        if(allStraight)
          longest[longestIdx(i, path)] = k;
        else
          break;
      }
    }

    if(path.vertexFront().isBoundary())
      longest[longestIdx(-1, path)] = longest[0];
    if(n > 1 && path.vertexBack().isBoundary())
    {
      BOOST_FOREACH(ptrdiff_t & i, longest)
      {
        if(i == n - 1)
          i = n;
      }
      longest[n - 1] = n;
    }

    return longest;
  }

template< typename MapTraits>
  bool
  VoronoiPathTracer< MapTraits>::isStraight(const Path & path, size_t i,
      size_t k) const
  {
    SSLIB_ASSERT(path.inRange(i));
    SSLIB_ASSERT(path.inRange(k));

    // Paths of length <= 3 are always straight
    if(path.forwardDist(i, k) <= 3)
      return true;

    // The start and end Delaunay edges
    const typename Delaunay::Edge & dgE1 = path.edge(i).delaunayEdge();
    const typename Delaunay::Edge & dgE2 =
        path.edge(path.wrapIndex(k - 1)).delaunayEdge();

    const Point v_i[] =
      { dgE1.first->vertex((dgE1.second + 1) % 3)->point(), dgE1.first->vertex(
          (dgE1.second + 2) % 3)->point() };
    const Point v_k[] =
      { dgE2.first->vertex((dgE2.second + 1) % 3)->point(), dgE2.first->vertex(
          (dgE2.second + 2) % 3)->point() };

    // Construct the 4 segments between the two endpoints of the two Delaunay edges
    Segment ik[4];
    for(size_t l = 0; l < 2; ++l)
    {
      for(size_t m = 0; m < 2; ++m)
        ik[l * 2 + m] = Segment(v_i[l], v_k[m]);
    }

    DirectionChecker dirs;
    // No need to check paths of length 1 & 2, just seed the direction vector
    dirs.update(
        path.vertex(path.wrapIndex(i + 1)).point() - path.vertex(i).point());

    bool pathStraight;
    const ptrdiff_t first = path.safeIndex(i + 2);
    for(ptrdiff_t d = 0; d < path.forwardDist(first, k); ++d) // Inbetween vertices
    {
      pathStraight = false;
      const typename Path::Edge & edge = path.edge(
          path.wrapIndex(first + d - 1));
      dirs.update(
          path.vertex(edge.target()).point()
              - path.vertex(edge.source()).point());
      // Check 4-directions condition
      if(dirs.numDirections() == 4)
        break;

      const Segment edgeSegment = segment(edge.delaunayEdge());
      for(size_t l = 0; l < 4; ++l)
      {
#if CGAL_VERSION_NR < 1040300000
        if(!CGAL::intersection(ik[l], edgeSegment).empty())
#else
        if(CGAL::intersection(ik[l], edgeSegment))
#endif
        {
          // As soon as one of the segments intersects this Edge then we're
          // happy to move on
          pathStraight = true;
          break;
        }
      }
      if(!pathStraight)
        break;
    }

    // Add in the final direction vector
    dirs.update(
        path.vertex(k).point() - path.vertex(path.wrapIndex(k - 1)).point());
    if(dirs.numDirections() == 4)
      pathStraight = false;

    return pathStraight;
  }

template< typename MapTraits>
  bool
  VoronoiPathTracer< MapTraits>::isStraight(
      const typename Delaunay::Edge & startEdge,
      const typename Delaunay::Edge & endEdge, const Path & path,
      const size_t pathEdgeBegin, const size_t pathEdgeLast,
      DirectionChecker * const dirs) const
  {
    SSLIB_ASSERT(pathEdgeBegin < path.numEdges());
    SSLIB_ASSERT(pathEdgeLast < path.numEdges());
    SSLIB_ASSERT(pathEdgeBegin <= pathEdgeLast);

    const Point v_i[] =
      { startEdge.first->vertex((startEdge.second + 1) % 3)->point(),
          startEdge.first->vertex((startEdge.second + 2) % 3)->point() };
    const Point v_k[] =
      { endEdge.first->vertex((endEdge.second + 1) % 3)->point(),
          endEdge.first->vertex((endEdge.second + 2) % 3)->point() };

    // Construct the 4 segments between the two endpoints of the two Delaunay edges
    Segment ik[4];
    for(size_t l = 0; l < 2; ++l)
    {
      for(size_t m = 0; m < 2; ++m)
        ik[l * 2 + m] = Segment(v_i[l], v_k[m]);
    }

    bool pathStraight;
    for(size_t j = pathEdgeBegin; j <= pathEdgeLast; ++j) // Path edges to check
    {
      pathStraight = false;
      const typename Path::Edge & edge = path.edge(j);
      dirs->update(
          path.vertex(edge.target()).point()
              - path.vertex(edge.source()).point());
      // Check 4-directions condition
      if(dirs->numDirections() == 4)
        break;

      const Segment edgeSegment = segment(edge.delaunayEdge());
      for(size_t l = 0; l < 4; ++l)
      {
#if CGAL_VERSION_NR < 1040300000
        if(!CGAL::intersection(ik[l], edgeSegment).empty())
#else
        if(CGAL::intersection(ik[l], edgeSegment))
#endif
        {
          // As soon as one of the segments intersects this Edge then we're
          // happy to move on
          pathStraight = true;
          break;
        }
      }
      if(!pathStraight)
        break;
    }

    return pathStraight;
  }

template< typename MapTraits>
  typename VoronoiPathTracer< MapTraits>::PossiblePath
  VoronoiPathTracer< MapTraits>::findOptimumPath(const Path & path,
      const std::vector< ptrdiff_t> & longest) const
  {
    const ptrdiff_t n = static_cast< ptrdiff_t>(path.numVertices());
    if(n == 0)
      return PossiblePath();
    else if(n == 1)
    {
      PossiblePath poss;
      poss.push_back(0);
      return poss;
    }

    ptrdiff_t i, j;

    // Calculate clipped paths i.e. 1 less at start and end then longest path
    std::vector< ptrdiff_t> clip0(n);
    for(i = 0; i < n; ++i)
    {
      size_t c = longestIdx(longest[longestIdx(i - 1, path)] - 1, path);
      // Special case where path is only straight from v_i-1,v_i,v_i+1
      // in which case we need to allow the path to 'continue' past v_i
      // and on to v_i+1
      if(c == i)
        c = longestIdx(i + 1, path);
      clip0[i] = c < i ? n : c;
    }

    const ptrdiff_t endVertex = path.isClosed() ? n : n - 1;
    // Calculate seg0[j] = longest path index from 0 with j segments
    std::vector< ptrdiff_t> seg0(endVertex + 1);
    for(i = 0, j = 0; i < endVertex; ++j)
    {
      SSLIB_ASSERT(j < seg0.size());
      SSLIB_ASSERT(i < clip0.size());
      seg0[j] = i;
      i = clip0[i];
    }
    seg0[j] = endVertex;
    const ptrdiff_t m = j; // The number of segments in the full path

    // Calculate backwards clipped paths
    std::vector< ptrdiff_t> clip1(endVertex + 1);
    for(j = 1, i = 0; i < endVertex; ++i)
    {
      while(j <= clip0[i])
      {
        clip1[j] = i;
        ++j;
      }
    }

    // Calculate seg1[j] = longest path index to n with m - j segments
    std::vector< ptrdiff_t> seg1(endVertex + 1);
    for(i = endVertex, j = m; j > 0; --j)
    {
      seg1[j] = i;
      i = clip1[i];
    }
    seg1[0] = 0;

    std::vector< double> penalties(endVertex + 1);
    std::vector< ptrdiff_t> prev(endVertex + 1);
    penalties[0] = 0.0;
    for(j = 1; j <= m; j++)
    {
      for(i = seg1[j]; i <= seg0[j]; i++)
      {
        double best = -1.0;
        for(ptrdiff_t k = seg0[j - 1]; k >= clip1[i]; k--)
        {
          const double thispen = penalty(path, Subpath(k, i)) + penalties[k];
          if(best < 0 || thispen < best)
          {
            prev[i] = k;
            best = thispen;
          }
        }
        penalties[i] = best;
      }
    }

    // Get the shortest path
    PossiblePath optimal(m + 1);
    optimal.back() = path.isClosed() ? 0 : n - 1;
    for(i = endVertex, j = m - 1; i > 0; --j)
    {
      i = prev[i];
      optimal[j] = i;
    }

    return optimal;
  }

template< typename MapTraits>
  double
  VoronoiPathTracer< MapTraits>::penalty(const Path & fullPath,
      const Subpath & subpath) const
  {
    SSLIB_ASSERT(subpath.first < subpath.second);

    const size_t n = subpath.second - subpath.first;

    const Segment v_ij(fullPath.vertex(subpath.first).point(),
        fullPath.vertex(subpath.second).point());
    const double lenSq = CGAL::to_double(v_ij.squared_length());

    double sumSq = 0.0;
    for(size_t k = subpath.first; k < subpath.second; ++k)
      sumSq += CGAL::to_double(
          CGAL::squared_distance(fullPath.vertex(k).point(), v_ij));

    return lenSq * (1.0 / static_cast< double>(n)) * sumSq;
  }

template< typename MapTraits>
  void
  VoronoiPathTracer< MapTraits>::adjustVertices(Path * const path) const
  {
    const std::vector< size_t> & optimal = path->getOptimalPath();
    typename Path::Curve & curve = path->curve();

    const size_t n = optimal.size();
    SSLIB_ASSERT(n >= 2);

    // TODO: Check this
    if(/*(path->isClosed() && n == path->numVertices())
        ||*/ (path->isClosed() && n == path->numVertices() + 1))
    {
      // If the optimal path is just the path itself, then insert
      // all vertices
      for(size_t i = 0; i < n - 1; ++i)
        curve.pushBack(path->vertex(optimal[i]).point());
    }
    else
    {
      if(!path->isClosed())
        curve.pushBack(path->vertexFront().point());

      size_t i, j, k;
      for(ptrdiff_t d = path->isClosed() ? 0 : 1; d < n - 1; ++d)
      {
        if(path->isClosed())
        {
          if(d == 0)
            // Need to do this because optimal stores the start and end
            // points explicitly, even if they are the same
            i = optimal[mod(d - 2, n)];
          else
            i = optimal[mod(d - 1, n)];
        }
        else
          i = optimal[d - 1];

        j = optimal[d];
        k = optimal[d + 1];

        typename CGAL::Linear_algebraCd< FT>::Matrix Q = path->quadraticForm(i,
            j);
        Q += path->quadraticForm(j, k);

        curve.pushBack(joinLines(Q, path->vertex(j).domain()));
      }

      if(!path->isClosed())
        curve.pushBack(path->vertexBack().point());
    }
  }

template< typename MapTraits>
  void
  VoronoiPathTracer< MapTraits>::optimiseBoundaryVertices(
      PathArrangement * const arr) const
  {
    typedef typename PathArrangement::BoundaryVertices BoundaryVertices;
    typedef typename BoundaryVertices::const_iterator BoundaryVertexIterator;
    typedef std::pair< BoundaryVertexIterator, BoundaryVertexIterator> BoundaryVertexRange;

    const BoundaryVertices & boundary = arr->getBoundaryVertices();

    for(BoundaryVertexIterator it = boundary.begin(), end = boundary.end();
        it != end; /*increment in loop body*/)
    {
      const BoundaryVertexRange range(it, boundary.upper_bound(it->first));

      typename CGAL::Linear_algebraCd< FT>::Matrix Q(3, 3, 0.0);
      BOOST_FOREACH(typename BoundaryVertices::const_reference v, range)
      {
        const typename PathArrangement::VertexHandle & vtx = v.second;
        const Path * const path = vtx.path();
        const std::vector< size_t> & optimal = path->getOptimalPath();
        SSLIB_ASSERT(optimal.size() >= 2);

        if(vtx.idx() == 0)
          Q += path->quadraticForm(vtx.idx(), optimal[1]);
        else
          Q += path->quadraticForm(optimal[optimal.size() - 2], vtx.idx());
      }

      const typename PathArrangement::VertexHandle & firstVtx =
          range.first->second;
      const Point joined = joinLines(Q, firstVtx->domain());

      // Now save the optimal point to all the meeting vertices
      BOOST_FOREACH(typename BoundaryVertices::const_reference v, range)
      {
        const typename PathArrangement::VertexHandle & vtx = v.second;
        Path * const path = vtx.path();

        if(vtx.idx() == 0)
          path->curve().vertexFront().point() = joined;
        else
          path->curve().vertexBack().point() = joined;
      }

      it = range.second;
    }
  }

template< typename MapTraits>
  void
  VoronoiPathTracer< MapTraits>::optimiseMeetingVertices(
      PathArrangement * const arr) const
  {
    typedef typename PathArrangement::MeetingVertices MeetingVertices;
    typedef typename MeetingVertices::const_iterator MeetingVertexIterator;
    typedef std::pair< MeetingVertexIterator, MeetingVertexIterator> MeetingVertexRange;

    const MeetingVertices & meeting = arr->getMeetingVertices();

    for(MeetingVertexIterator it = meeting.begin(), end = meeting.end();
        it != end; /*increment in loop body*/)
    {
      const MeetingVertexRange range(it, meeting.upper_bound(it->first));

      typename CGAL::Linear_algebraCd< FT>::Matrix Q(3, 3, 0.0);
      BOOST_FOREACH(typename MeetingVertices::const_reference v, range)
      {
        const typename PathArrangement::VertexHandle & vtx = v.second;
        const Path * const path = vtx.path();
        const std::vector< size_t> & optimal = path->getOptimalPath();
        SSLIB_ASSERT(optimal.size() >= 2);

        if(vtx.idx() == 0)
          Q += path->quadraticForm(vtx.idx(), optimal[1]);
        else
          Q += path->quadraticForm(optimal[optimal.size() - 2], vtx.idx());
      }

      const typename PathArrangement::VertexHandle & firstVtx =
          range.first->second;
      const Point joined = joinLines(Q, firstVtx->domain());

      BOOST_FOREACH(typename MeetingVertices::const_reference v, range)
      {
        const typename PathArrangement::VertexHandle & vtx = v.second;
        Path * const path = vtx.path();

        if(vtx.idx() == 0)
          path->curve().vertexFront().point() = joined;
        else
          path->curve().vertexBack().point() = joined;
      }

      it = range.second;
    }
  }

template< typename MapTraits>
  void
  VoronoiPathTracer< MapTraits>::smooth(Path * const path) const
  {
    using CGAL::squared_distance;

    const std::vector< size_t> & optimal = path->getOptimalPath();
    const size_t start = path->isClosed() ? 0 : 1, end = optimal.size() - 2;
    const ptrdiff_t n = path->isClosed() ? optimal.size() - 1 : optimal.size();

    typename Path::Curve & curve = path->curve();
    Vector v_ik, perp;
    Point b_i, b_k, closest, c, p4;
    FT alpha, gamma;

    if(!path->isClosed())
      curve.vertexFront().end = detail::interval(0.5, curve.vertex(0).point(),
          curve.vertex(1).point());

    for(size_t j = start; j <= end; ++j)
    {
      const size_t i = mod(j - 1, n);
      const size_t k = mod(j + 1, n);

      const Point & p_i = curve.vertex(i).point();
      const Point & p_j = curve.vertex(j).point();
      const Point & p_k = curve.vertex(k).point();

      b_i = detail::interval(0.5, p_i, p_j);
      b_k = detail::interval(0.5, p_j, p_k);

      v_ik = p_k - p_i;

      closest = detail::closestPoint(Segment(b_i, b_k),
          path->vertex(optimal[i]).domain());
      gamma = CGAL::sqrt(
          squared_distance(p_i, closest) / squared_distance(p_i, p_j));
      alpha = 4 * gamma / 3;

      curve.vertex(j).end = detail::interval(0.5, p_k, p_j); // TEMP: Startpoint

      if(alpha <= 1)
      {
        if(alpha < 0.55)
          alpha = 0.55;
        else if(alpha > 1)
          alpha = 1;

        BezierCurve< K> bezier;
        bezier.alpha = alpha;

        bezier.control[0] = b_i; // Startpoint
        bezier.control[1] = detail::interval(0.5 * (1 + alpha), p_i, p_j); // Control point 1 & 2
        bezier.control[2] = detail::interval(0.5 * (1 + alpha), p_k, p_j);
        bezier.control[3] = detail::interval(0.5, p_k, p_j); // Endpoint

        curve.vertex(j).setBezier(bezier);
      }
    }

    if(!path->isClosed())
      curve.vertexBack().end = curve.vertexBack().point();
  }

template< typename MapTraits>
  typename VoronoiPathTracer< MapTraits>::Point
  VoronoiPathTracer< MapTraits>::joinLines(
      const typename CGAL::Linear_algebraCd< FT>::Matrix & Q,
      const Polygon & boundary) const
  {
    typedef CGAL::Linear_algebraCd< FT> Linalg;
    typedef typename Linalg::Matrix AlgMatrix;
    typedef typename Linalg::Vector AlgVector;

    Point best = CGAL::centroid(boundary.vertices_begin(),
        boundary.vertices_end());
    FT minDistSq = quadDist(best, Q);

    AlgMatrix nnT(2, 2);
    for(size_t i = 0; i < 2; ++i)
    {
      for(size_t j = 0; j < 2; ++j)
        nnT(i, j) = Q(i, j);
    }

    AlgVector npTn(2);
    npTn[0] = -Q(0, 2);
    npTn[1] = -Q(1, 2);

    AlgVector x;
    FT D;
    if(!Linalg::linear_solver(nnT, npTn, x, D))
      return best; // TODO: Fix this, it probably happens because the lines are parallel
    const Point intersection(x[0] * (1.0 / D), x[1] * (1.0 / D));

    // Now check that it isn't outside the boundary
    if(boundary.bounded_side(intersection) != CGAL::ON_UNBOUNDED_SIDE)
      return intersection;

    FT distSq;
    Point bound;
    for(typename Polygon::Edge_const_iterator it = boundary.edges_begin(), end =
        boundary.edges_end(); it != end; ++it)
    {
      const Line boundaryLine = it->supporting_line();

      // Parametric parameters
      const Point p0 = it->source();

      const Vector lineVec = it->to_vector();
      const Vector ortho = lineVec.perpendicular(CGAL::COUNTERCLOCKWISE);

      // Project a line orthogonal to the edge toward the intersection point
      // (i.e. the closest approach) but limit it to be on the boundary
      FT t = ortho * (intersection - CGAL::ORIGIN);
      t = CGAL::min(CGAL::max(t, FT(0.0)), FT(1.0));
      bound = p0 + t * lineVec;

      distSq = quadDist(bound, Q);
      if(distSq < minDistSq)
      {
        minDistSq = distSq;
        best = bound;
      }
    }

    return best;
  }

template< typename MapTraits>
  typename VoronoiPathTracer< MapTraits>::Polygon
  VoronoiPathTracer< MapTraits>::surroundingBoundary(
      const typename Delaunay::Edge & edge, const Voronoi & voronoi) const
  {
    const typename Voronoi::Halfedge_handle he = voronoi.dual(edge);
    SSLIB_ASSERT(!he->is_unbounded());

    const typename Voronoi::Vertex_handle source = he->source();
    const typename Voronoi::Vertex_handle target = he->target();

    Polygon poly;
    typename Voronoi::Halfedge_around_vertex_circulator start =
        voronoi.incident_halfedges(target, he);
    typename Voronoi::Halfedge_around_vertex_circulator cl = start;
    do
    {
      poly.push_back(cl->face()->dual()->point());
      ++cl;
    } while(cl != start);

    cl = voronoi.incident_halfedges(source, he->twin());
    typename Voronoi::Halfedge_around_vertex_circulator end = cl;
    --end;
    ++cl;
    do
    {
      poly.push_back(cl->face()->dual()->point());
      ++cl;
    } while(cl != end);

    return poly;
  }

template< typename MapTraits>
  typename VoronoiPathTracer< MapTraits>::FT
  VoronoiPathTracer< MapTraits>::quadDist(const Point & p,
      const typename CGAL::Linear_algebraCd< FT>::Matrix & Q) const
  {
    typename CGAL::Linear_algebraCd< FT>::Matrix v(3, 1);
    v(0, 0) = p.x();
    v(1, 0) = p.y();
    v(2, 0) = 1;

    return (CGAL::Linear_algebraCd< FT>::transpose(v) * Q * v)(0, 0);
  }

}

template< typename MapTraits>
  template< class PointIterator>
    typename MapTraits::Arrangement
    VoronoiPathTracer< MapTraits>::processPath(PointIterator first,
        PointIterator last)
    {
      typedef detail::VoronoiPathTracer< MapTraits> Tracer;

      // Create the Voronoi diagram for the tracer to work on
      typename Tracer::Delaunay tempDelaunay;
      tempDelaunay.insert(first, last);
      typename Tracer::Voronoi voronoi(tempDelaunay, true);

      // Create the path arrangement
      Tracer tracer;
      const typename Tracer::PathArrangement & pathArrangement =
          tracer.generatePaths(voronoi);

      // Finally create the map from the arrangement
      return analysis::toMap< MapTraits>(pathArrangement);
    }

}
}

#endif /* VORONOI_PATH_TRACER_DETAIL_H */
