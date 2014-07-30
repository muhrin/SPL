/*
 * VoronoiEdgeTracer.h
 *
 *  Created on: Oct 15, 2013
 *      Author: Martin Uhrin
 */

#ifndef VORONOI_EDGE_TRACER_H
#define VORONOI_EDGE_TRACER_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_USE_CGAL

#include <map>
#include <set>

#include <CGAL/CORE_Expr.h>
#include <CGAL/Arrangement_2.h>
#include <CGAL/Arr_extended_dcel.h>
#include <CGAL/Arr_segment_traits_2.h>

#include <CGAL/Exact_predicates_exact_constructions_kernel_with_sqrt.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Voronoi_diagram_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_policies_2.h>

#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Voronoi_diagram_2.h>

#include "spl/utility/Range.h"

// FORWARD DECLARATIONS ///////

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {
namespace arrangement_data {
template< typename LabelType>
  struct Vertex
  {
    Vertex():
      maxDisplacement(0),
      isOuterBoundary(false)
    {
    }
    double maxDisplacement;
    bool isOuterBoundary;
  };

template< typename LabelType>
  struct Halfedge
  {
    Halfedge():
      label(),
      isOuterBoundary(false)
    {
    }
    LabelType label;
    bool isOuterBoundary;
  };

template< typename LabelType>
  struct Face
  {
    LabelType label;
  };
} // namespace arrangement_data

template< typename LabelType, class VertexDataType = arrangement_data::Vertex<
    LabelType>, class HalfedgeDataType = arrangement_data::Halfedge< LabelType>,
    class FaceDataType = arrangement_data::Face< LabelType> >
  class VoronoiEdgeTracer
  {
    typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
    //typedef CGAL::Exact_predicates_exact_constructions_kernel_with_sqrt K;

    // Arrangements stuff
    typedef CGAL::Arr_segment_traits_2< K> ArrTraits;
    typedef CGAL::Arr_extended_dcel< ArrTraits, VertexDataType,
        HalfedgeDataType, FaceDataType> Dcel;

    // typedefs for defining the adaptor
    typedef CGAL::Triangulation_vertex_base_with_info_2< LabelType, K> Vb;
    typedef CGAL::Triangulation_data_structure_2< Vb> Tds;

  public:
    typedef K::Point_2 Point;
    typedef CGAL::Arrangement_2< ArrTraits, Dcel> Arrangement;
    typedef CGAL::Delaunay_triangulation_2< K, Tds> Delaunay;

  private:
    typedef ArrTraits::Segment_2 ArrSegment;

    typedef CGAL::Delaunay_triangulation_adaptation_traits_2< Delaunay> AT;
    typedef CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<
        Delaunay> AP;

  public:
    typedef CGAL::Voronoi_diagram_2< Delaunay, AT, AP> Voronoi;

  private:
    typedef ::std::map< typename Voronoi::Vertex_handle,
        typename Arrangement::Vertex_handle> VertexMap;
    typedef ::std::set< typename Voronoi::Delaunay_edge> DelaunayEdgeSet;
    typedef ::std::map< typename Voronoi::Halfedge_handle,
        typename Arrangement::Vertex_handle> BoundaryVertexMap;

    struct TracingData
    {
      TracingData(const Voronoi & _voronoi) :
          voronoi(_voronoi)
      {
      }

      const Voronoi & voronoi;
      VertexMap vertexMap;
      BoundaryVertexMap boundaryVertices;
      DelaunayEdgeSet toVisit;
      DelaunayEdgeSet boundaryEdges;
      ::CGAL::Polygon_2<K> boundary;
    };

  public:
    VoronoiEdgeTracer(const Voronoi & voronoi, const bool splitSharedVertices =
        true);

    Arrangement &
    getArrangement();

    const Arrangement &
    getArrangement() const;

  private:
    class VertexSplitter
    {
    public:
      typedef ::spl::utility::OrderedPair< size_t> Edge;
    private:
      typedef ::std::map< Edge, LabelType> Edges;
    public:
      typedef typename Edges::const_iterator EdgeIterator;

      VertexSplitter(const size_t degree);
      void
      addEdge(const size_t i, const size_t j, const LabelType & label);

      EdgeIterator
      edgesBegin() const;
      EdgeIterator
      edgesEnd() const;

      bool
      noEdges() const;
      size_t
      numEdges() const;
      void
      resolveCollisions();

    private:
      const size_t degree_;
      Edges edges_;
    };

    class SplitVertex
    {
    public:
      // Halfedge data: first - the halfedge in the ccw direction,
      // second - the twin halfedge i.e. on the cw side of the edge
      typedef ::std::pair<HalfedgeDataType, HalfedgeDataType> HalfedgeData;
    private:
      typedef ::std::vector<
          typename Arrangement::Halfedge_around_vertex_circulator> Halfedges;
      typedef ::std::vector< typename Arrangement::Vertex_handle> Neighbours;
      typedef ::std::vector< HalfedgeData> HalfedgesData;
      typedef typename K::Vector_2 Vector_2;
    public:
      typedef typename Neighbours::const_iterator NeighbourIterator;
      typedef typename Halfedges::const_iterator HalfedgeIterator;

      SplitVertex();

      void
      addHalfedge(
          const typename Arrangement::Halfedge_around_vertex_circulator & halfedge);

      size_t
      numNeighbours() const;
      NeighbourIterator
      neighboursBegin() const;
      NeighbourIterator
      neighboursEnd() const;

      HalfedgeIterator
      halfedgesBegin() const;
      HalfedgeIterator
      halfedgesEnd() const;

      const HalfedgeData &
      getHalfedgeData(const size_t idx) const;

      Vector_2
      meanPos() const;
    private:
      Halfedges halfedges_;
      Neighbours neighbours_;
      HalfedgesData halfedgeData_;
      K::Vector_2 newPos_;
    };

    void
    initArrangement(TracingData & tracingData);
    void
    traceEdge(TracingData & tracingData,
        typename DelaunayEdgeSet::const_iterator edgeIt);
    void
    traceEdge(TracingData & tracingData,
        const typename Voronoi::Halfedge_handle & halfEdge,
        const typename VertexMap::const_iterator source);
    bool
    isBoundaryEdge(const typename Delaunay::Edge & edge) const;
    void
    createBoundary(TracingData * const tracingData) const;
    void
    splitSharedVertices();
    void
    splitEdges();
    void
    populateFaceLabels();
    Point
    midpoint(const typename Delaunay::Edge & edge) const;

    Arrangement arrangement_;
  };

}
}

#include "spl/analysis/detail/VoronoiEdgeTracer.h"

#endif /* SPL_USE_CGAL */
#endif /* VORONOI_EDGE_TRACER_H */
