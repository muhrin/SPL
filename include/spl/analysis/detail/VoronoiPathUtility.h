/*
 * VoronoiPathUtility.h
 *
 *  Created on: Feb 18, 2014
 *      Author: Martin Uhrin
 */

#ifndef VORONOI_PATH_UTILITY_DETAIL_H
#define VORONOI_PATH_UTILITY_DETAIL_H

// INCLUDES ///////////////////

#include "spl/SSLibAssert.h"

#include <map>

#include <boost/foreach.hpp>
#include <boost/range/iterator_range.hpp>

#include <CGAL/Arr_observer.h>
#include <CGAL/Point_2.h>
#include <CGAL/Segment_2.h>
#include <CGAL/Cartesian_converter.h>

#define DEBUG_DONT_CREATE_ARRANGEMENT

// FORWARD DECLARATIONS ///////

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {
namespace detail {

template< typename Map, typename Label>
  class MapLabeller : CGAL::Arr_observer< Map>
  {
  public:
    typedef std::pair< boost::optional< Label>, boost::optional< Label> > EdgeLabels;

    MapLabeller(Map * const map) :
        myMap(map)
    {
      this->attach(*myMap);
    }
    virtual
    ~MapLabeller()
    {
      this->detach();
    }

    void
    setEdgeLabels(const EdgeLabels & labels)
    {
      myEdgeLabels = labels;
    }

  private:
    virtual void
    after_create_edge(typename Map::Halfedge_handle e)
    {
      assignLabels(myEdgeLabels, e);
    }

    virtual void
    before_split_edge(typename Map::Halfedge_handle e,
        typename Map::Vertex_handle v,
        const typename Map::X_monotone_curve_2 & c1,
        const typename Map::X_monotone_curve_2 & c2)
    {
      // Save the labels from the edge about to be split
      mySplitEdgeLabels.first = e->data().label;
      mySplitEdgeLabels.second = e->twin()->data().label;
    }

    virtual void
    after_split_edge(typename Map::Halfedge_handle e1,
        typename Map::Halfedge_handle e2)
    {
      // Assign the labels from the edge before splitting to these new edges
      assignLabels(mySplitEdgeLabels, e1);
      assignLabels(mySplitEdgeLabels, e2);
    }

    void
    assignLabels(const EdgeLabels & labels,
        typename Map::Halfedge_handle e) const
    {
      // Check which way around we got the halfedge.  The labels will always
      // apply relative to the direction of the curve i.e. from
      // curve.source() -> curve.target():
      //
      // labels.first = halfedge
      // labels.second = halfedge.twin()

      if(e->curve().source().is_same(e->source()->point()))
      {
        e->data().label = labels.first;
        e->twin()->data().label = labels.second;
      }
      else // must have the halfedge going the other way
      {
        e->data().label = labels.second;
        e->twin()->data().label = labels.first;
      }
    }

    Map * const myMap;
    EdgeLabels myEdgeLabels;
    EdgeLabels mySplitEdgeLabels;
    boost::optional<
        std::pair< typename Map::Vertex_handle, typename Map::Vertex_handle> > myUpcomingEdge;
  };

template< typename MapTraits, typename VD>
  struct MapBuilder
  {
    typedef typename MapTraits::Label Label;
    typedef typename MapTraits::Arrangement Map;
    typedef VD Voronoi;
    typedef typename VD::Delaunay_graph Delaunay;
    typedef typename MapTraits::ArrPoint Point;
    typedef VoronoiPathArrangement< VD> PathArrangement;
    typedef typename PathArrangement::Path Path;
    typedef MapLabeller< Map, Label> EdgeLabeller;

    typedef std::map< typename VD::Vertex_handle, typename Map::Vertex_handle> MeetingVertices;
    typedef std::map< typename Delaunay::Edge, typename Map::Vertex_handle> BoundaryVertices;
    typedef std::map< const Path *, std::vector< typename Map::Vertex_handle> > PathVertices;

    class PathDrawer
    {
      typedef typename Path::Point PathPoint;

    public:
      explicit
      PathDrawer(Map * const map) :
          myMap(map)
      {
      }
      void
      moveTo(const PathPoint & p)
      {
        //std::cout << "(Path.MOVETO, " << point(p) << "),\n";
        updatePos(TO_EXACT(p));
      }
      void
      lineTo(const PathPoint & p)
      {
        //std::cout << "(Path.LINETO, " << point(p) << "),\n";

        const Point pts[] =
          { myPos, TO_EXACT(p) };
        if(pts[1] != myPos)
        {
#ifndef DEBUG_DONT_CREATE_ARRANGEMENT
          CGAL::insert(*myMap, Curve(pts, pts + 2));
#endif
          updatePos(pts[1]);
        }
      }
      template< typename InputIterator>
        void
        curveTo(InputIterator first, InputIterator last)
        {
//          for(InputIterator it = first; it != last; ++it)
//            std::cout << "(Path.CURVE4, " << point(*it) << "),\n";

          std::vector< Point> pts(1, myPos);
          BOOST_FOREACH(const typename Path::Point & p,
              boost::make_iterator_range(first, last))
            pts.push_back(TO_EXACT(p));

#ifndef DEBUG_DONT_CREATE_ARRANGEMENT
          CGAL::insert(*myMap, Curve(pts.begin(), pts.end()));
#endif
          updatePos(pts.back());
        }

    private:
      typedef typename MapTraits::ArrTraits::Curve_2 Curve;
      typedef CGAL::Cartesian_converter< typename Delaunay::Geom_traits,
          typename MapTraits::RatKernel> IkToEk;

      // Use this to convert the number type used by the Voronoi diagram paths
      // to the rational type used by the arrangement
      static const IkToEk TO_EXACT;

      void
      updatePos(const Point & p)
      {
        myPos = p;
      }

      std::string
      point(const PathPoint & pt)
      {
        std::stringstream ss;
        ss << "(" << pt.x() << ", " << pt.y() << ")";
        return ss.str();
      }

      Map * const myMap;
      Point myPos;
    };

    void
    placePathEdges(const Path & path, Map * const map)
    {
      typedef typename EdgeLabeller::EdgeLabels EdgeLabels;
      typedef typename Path::Point PathPoint;

      // Set the edge labels
      EdgeLabeller labeller(map);
      if(path.numEdges() != 0)
        labeller.setEdgeLabels(EdgeLabels(path.leftLabel(), path.rightLabel()));

      const typename Path::Curve & curve = path.curve();
      PathDrawer draw(map);

      if(path.isClosed())
        draw.moveTo(curve.vertexBack().end);
      else
        draw.moveTo(curve.vertexFront().point());
      for(size_t i = 0; i < curve.numVertices() - 1; ++i)
      {
        const typename Path::Curve::Vertex & vi = curve.vertex(i);

        if(vi.getBezier())
        {
          const PathPoint (&control)[3] = vi.getBezier()->control;
          draw.curveTo(control, control + 3);
        }
        else
        {
          draw.lineTo(vi.point());
          draw.lineTo(vi.end);
        }
      }
      const typename Path::Curve::Vertex & lastVtx = curve.vertexBack();
      if(lastVtx.getBezier())
      {
        const PathPoint (&control)[3] = lastVtx.getBezier()->control;
        draw.curveTo(control, control + 3);
      }
      else
      {
        draw.lineTo(lastVtx.point());
        if(path.isClosed())
          draw.lineTo(lastVtx.point());
      }
    }

    void
    createBoundary(const PathArrangement & arrangement, Map * const map)
    {
      // Here we perform a counter-clockwise circulation of the infinite
      // vertex.  This corresponds to a clockwise circulation of the convex
      // hull of the Delaunay triangulation.

      typedef VD Voronoi;
      typedef typename Voronoi::Delaunay_graph Delaunay;
      typedef typename Delaunay::Edge Edge;
      typedef typename Delaunay::Face_circulator FaceCirculator;
      typedef typename EdgeLabeller::EdgeLabels EdgeLabels;

      const Voronoi & voronoi = arrangement.getVoronoi();
      const Delaunay & delaunay = voronoi.dual();

      const typename Delaunay::Vertex_handle infiniteVertex =
          delaunay.infinite_vertex();

      FaceCirculator start = delaunay.incident_faces(infiniteVertex);
      FaceCirculator cl = start;
      do
      {
        if(spansBoundary< Delaunay>(Edge(cl, cl->index(infiniteVertex))))
          break;
        ++cl;
      } while(cl != start);
      start = cl; // Start at the newly found boundary edge (or the original start)

      // Set up the labels
      EdgeLabeller labeller(map);
      labeller.setEdgeLabels(
          EdgeLabels(boost::optional< Label>(),
              cl->vertex(delaunay.ccw(cl->index(infiniteVertex)))->info()));

      // Create the drawer
      PathDrawer draw(map);
      draw.moveTo(cl->vertex(delaunay.ccw(cl->index(infiniteVertex)))->point());

      const typename PathArrangement::BoundaryVerticesConst & boundaryVertices =
          arrangement.getBoundaryVertices();
      typename PathArrangement::BoundaryVerticesConst::const_iterator it;
      do
      {
        const Edge edge(cl, cl->index(infiniteVertex));

        if(spansBoundary< Delaunay>(edge))
        {
          // Viewing a vertical edge with the infinite vertex (i) to its right
          // that span a boundary it looks like this:
          //
          //   |.  <- ccw(i)
          // --|   <- The boundary edge that intersects the convex hull
          //   |.  <- cw(i)
          it = boundaryVertices.find(edge);
          SSLIB_ASSERT(it != boundaryVertices.end());

          const typename Path::Point & p1 =
              it->second.idx() == 0 ?
                  it->second.path()->curve().vertexFront().point() :
                  it->second.path()->curve().vertexBack().point();

          // Insert the edge segment into the arrangement
          draw.lineTo(p1);

          labeller.setEdgeLabels(
              EdgeLabels(boost::optional< Label>(),
                  cl->vertex(delaunay.cw(edge.second))->info()));
        }

        draw.lineTo(cl->vertex(delaunay.cw(edge.second))->point());

        ++cl;
      } while(cl != start);
    }

    void
    populateFaceLabels(Map * const map) const
    {
      BOOST_FOREACH(typename Map::Face & f,
          boost::make_iterator_range(map->faces_begin(), map->faces_end()))
      {
        if(f.has_outer_ccb())
        {
          const typename Map::Ccb_halfedge_circulator start = f.outer_ccb();
          typename Map::Ccb_halfedge_circulator cl = start;
          do
          {
            if(cl->data().label)
            {
              f.data().label = cl->data().label;
              break;
            }
            ++cl;
          } while(cl != start);
        }
      }
    }

    MeetingVertices meeting;
    BoundaryVertices boundary;
    PathVertices pathVertices;
  };

template< typename MapTraits, typename VD>
  const typename MapBuilder< MapTraits, VD>::PathDrawer::IkToEk MapBuilder<
      MapTraits, VD>::PathDrawer::TO_EXACT =  typename MapBuilder< MapTraits, VD>::PathDrawer::IkToEk();

}

template< class VD>
  std::pair< typename VoronoiLabel< VD>::Type, typename VoronoiLabel< VD>::Type>
  getBoundaryPair(const typename VD::Halfedge & he)
  {
    const typename VD::Delaunay_graph::Edge edge = he.dual();
    return std::make_pair(edge.first->vertex((edge.second + 1) % 3)->info(),
        edge.first->vertex((edge.second + 2) % 3)->info());
  }

template< class DG>
  typename BoundaryPair< typename DelaunayLabel< DG>::Type>::Type
  getSpanningPair(const typename DG::Edge edge)
  {
    return typename BoundaryPair< typename DelaunayLabel< DG>::Type>::Type(
        edge.first->vertex((edge.second + 2) % 3)->info(),
        edge.first->vertex((edge.second + 1) % 3)->info());
  }

template< class VD>
  bool
  isBoundary(const typename VD::Halfedge & he)
  {
    return spansBoundary< typename VD::Delaunay_graph>(he.dual());
  }

template< class DG>
  bool
  spansBoundary(const typename DG::Edge & edge)
  {
    return edge.first->vertex((edge.second + 2) % 3)->info()
        != edge.first->vertex((edge.second + 1) % 3)->info();
  }

template< typename VD>
  CGAL::Polygon_2< typename VD::Delaunay_geom_traits>
  delaunayDomain(const typename VD::Vertex_handle & vtx, const VD & voronoi)
  {
    typedef typename VD::Delaunay_graph Delaunay;

    SSLIB_ASSERT(vtx->is_valid());

    const Delaunay & delaunay = voronoi.dual();
    CGAL::Polygon_2< typename VD::Delaunay_geom_traits> poly;

    typename VD::Halfedge_around_vertex_circulator start =
        voronoi.incident_halfedges(vtx);
    typename VD::Halfedge_around_vertex_circulator cl = start;
    typename Delaunay::Edge dgEdge;
    do
    {
      dgEdge = cl->dual();
      poly.push_back(
          dgEdge.first->vertex(delaunay.ccw(dgEdge.second))->point());

      ++cl;
    } while(cl != start);

    return poly;
  }

template< typename MapTraits, typename VD>
  typename MapTraits::Arrangement
  toMap(const VoronoiPathArrangement< VD> & pathArrangement)
  {
    typedef typename MapTraits::Arrangement Map;
    typedef detail::MapBuilder< MapTraits, VD> Builder;

    Map map;
    Builder builder;

    // Connect the edges
    BOOST_FOREACH(const typename Builder::Path & path,
        boost::make_iterator_range(pathArrangement.pathsBegin(), pathArrangement.pathsEnd()))
      builder.placePathEdges(path, &map);

    builder.createBoundary(pathArrangement, &map);
    builder.populateFaceLabels(&map);

    SSLIB_ASSERT(map.is_valid());

    return map;
  }

}
}

#endif /* VORONOI_PATH_UTILITY_DETAIL_H */
