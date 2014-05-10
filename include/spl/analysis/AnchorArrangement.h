/*
 * AnchorArrangement.h
 *
 *  Created on: Oct 15, 2013
 *      Author: Martin Uhrin
 */

#ifndef ANCHOR_ARRANGEMENT_H
#define ANCHOR_ARRANGEMENT_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_WITH_CGAL
#include <map>
#include <vector>

#include <boost/optional.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <armadillo>

#include <CGAL/Polygon_2.h>

#include "spl/analysis/VoronoiEdgeTracer.h"

// FORWARD DECLARATIONS ///////

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {

template< typename LabelType>
  class AnchorArrangement;

class AnchorPoint
{
public:
  AnchorPoint(const size_t idx, const ::arma::vec2 & pos,
      const double maxDisplacement);

  const ::arma::vec2 &
  getAnchorPos() const;
  const ::arma::vec2 &
  getPos() const;
  void
  setPos(const ::arma::vec2 & newPos);

  double
  getMaxDisplacement() const;

  size_t
  idx() const;

private:
  const size_t idx_;
  const ::arma::vec2 anchorPos_;
  ::arma::vec2 pos_;
  const double maxDisplacement_;
};

template< typename LabelType>
  class AnchorArrangement
  {
    struct VertexData : public arrangement_data::Vertex< LabelType>
    {
      AnchorPoint * anchor;
    };
    struct HalfedgeData : public arrangement_data::Halfedge< LabelType>
    {
    };
    struct FaceData : public arrangement_data::Face< LabelType>
    {
    };

  public:
    typedef VoronoiEdgeTracer< LabelType, VertexData, HalfedgeData, FaceData> EdgeTracer;
    typedef typename EdgeTracer::Arrangement Arrangement;

  private:
    typedef ::boost::ptr_vector< AnchorPoint> Points;

  public:
    typedef typename Points::iterator PointsIterator;
    typedef typename Arrangement::Geometry_traits_2::Kernel CgalKernel;
    typedef typename ::CGAL::Point_2<CgalKernel> CgalPoint;
    typedef typename ::CGAL::Polygon_2< CgalKernel> FacePolygon;

    AnchorArrangement(EdgeTracer & edgeTracer);

    PointsIterator
    beginPoints();
    PointsIterator
    endPoints();
    size_t
    numPoints() const;
    AnchorPoint *
    getPoint(const size_t index);

    ::arma::mat
    getPointPositions() const;
    void
    setPointPositions(const ::arma::mat & pos);

    AnchorPoint *
    getAnchorPoint(const typename Arrangement::Vertex_const_handle & vertex);

    const Arrangement &
    getCgalArrangement() const;

    ::boost::optional< double>
    getFaceAnchorArea(const typename Arrangement::Face & face) const;

    FacePolygon
    getFacePolygon(const typename Arrangement::Face & face) const;

  private:
    void
    init();
    void
    generateArrangementVertices();
    void
    generateArrangementEdges();

    EdgeTracer & tracer_;
    Points points_;
  };

}
}

#include "spl/analysis/detail/AnchorArrangement.h"

#endif /* SPL_WITH_CGAL */
#endif /* ANCHOR_ARRANGEMENT_H */
