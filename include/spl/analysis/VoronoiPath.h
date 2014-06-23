/*
 * VoronoiPath.h
 *
 *  Created on: Mar 18, 2014
 *      Author: Martin Uhrin
 */

#ifndef VORONOI_PATH_H
#define VORONOI_PATH_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_WITH_CGAL

#include <map>
#include <vector>

#include <boost/optional.hpp>

#include <CGAL/Linear_algebraCd.h>

#include "spl/analysis/VoronoiPathUtility.h"
#include "spl/analysis/TraceCurve.h"

// FORWARD DECLARATIONS ///////

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {

template< typename VD>
  class VoronoiPath
  {
    typedef typename VoronoiLabel< VD>::Type Label;
  public:
    typedef VD Voronoi;
    typedef typename Voronoi::Delaunay_graph Delaunay;
    typedef typename Voronoi::Adaptation_traits::Point_2 Point;
    typedef typename Voronoi::Delaunay_geom_traits GeomTraits;
    typedef CGAL::Polygon_2< GeomTraits> Polygon;
    typedef CGAL::Segment_2< GeomTraits> Segment;
    typedef CGAL::Line_2< GeomTraits> Line;
    typedef std::pair< Line, typename GeomTraits::Point_2> LineAndCentroid;
    typedef typename CGAL::Linear_algebraCd< typename GeomTraits::FT>::Matrix Matrix;

    typedef TraceCurve< VD> Curve;

    class Vertex;
    class Edge;
  private:
    typedef std::vector< Vertex> Vertices;
    typedef std::vector< Edge> Edges;
    typedef typename Delaunay::Edge DelaunayEdge;
  public:
    typedef typename Vertices::iterator VertexIterator;
    typedef typename Vertices::const_iterator VertexConstIterator;
    typedef typename Edges::iterator EdgeIterator;
    typedef typename Edges::const_iterator EdgeConstIterator;
    typedef std::pair< size_t, size_t> Subpath;

    VoronoiPath();
    explicit
    VoronoiPath(const Voronoi & voronoi);
    VoronoiPath(const VoronoiPath & path);

    size_t
    push_back(const typename Voronoi::Halfedge_handle & he);
    size_t
    close(const typename Voronoi::Halfedge_handle & he);

    VertexConstIterator
    verticesBegin() const;
    VertexConstIterator
    verticesEnd() const;
    Vertex &
    vertex(const size_t index);
    const Vertex &
    vertex(const size_t index) const;
    const Vertex &
    vertexFront() const;
    const Vertex &
    vertexBack() const;

    EdgeConstIterator
    edgesBegin() const;
    EdgeConstIterator
    edgesEnd() const;
    Edge &
    edge(const size_t index);
    const Edge &
    edge(const size_t index) const;
    Edge &
    edgeFront();
    const Edge &
    edgeFront() const;
    Edge &
    edgeBack();
    const Edge &
    edgeBack() const;

    void
    reverse();

    size_t
    numVertices() const;

    size_t
    numEdges() const;

    bool
    isClosed() const;

    bool
    empty() const;

    const Voronoi *
    getVoronoi() const;

    bool
    inRange(const ptrdiff_t index) const;
    bool
    inRange(const Subpath & subpath) const;
    ptrdiff_t
    forwardDist(const ptrdiff_t i, const ptrdiff_t j) const;
    ptrdiff_t
    wrapIndex(const ptrdiff_t i) const;
    ptrdiff_t
    safeIndex(const ptrdiff_t i) const;

    const std::vector< size_t> &
    getOptimalPath() const;
    void
    setOptimalPath(const std::vector< size_t> & path);

    boost::optional< Label>
    leftLabel() const;
    boost::optional< Label>
    rightLabel() const;

    const LineAndCentroid &
    leastSquaresLine(const size_t i, const size_t j) const;
    const Matrix &
    quadraticForm(const size_t i, const size_t j) const;

    const Curve &
    curve() const;
    Curve &
    curve();

  private:
    void
    push_back(const typename Delaunay::Edge & edge);

    const Voronoi * myVoronoi;
    Vertices myVertices;
    Edges myEdges;
    Curve myCurve;
    std::vector< size_t> myOptimalPath;
    mutable std::map< Subpath, LineAndCentroid> mySubpathLines;
    mutable std::map< Subpath, Matrix> myQuadraticForms;
  };

template< typename VD>
  std::ostream &
  operator <<(std::ostream & os, const VoronoiPath< VD> & path);

}
}

#include "spl/analysis/detail/VoronoiPath.h"

#endif /* SPL_WITH_CGAL */
#endif /* VORONOI_PATH_H */
