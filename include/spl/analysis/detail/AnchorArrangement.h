/*
 * AnchorArrangement.h
 *
 *  Created on: Oct 15, 2013
 *      Author: Martin Uhrin
 */

#ifndef ANCHOR_ARRANGEMENT_DETAIL_H
#define ANCHOR_ARRANGEMENT_DETAIL_H

// INCLUDES ///////////////////
#include <boost/foreach.hpp>
#include <boost/range/iterator_range.hpp>

#include "spl/SSLibAssert.h"

// FORWARD DECLARATIONS ///////

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {

template< typename LabelType>
  AnchorArrangement< LabelType>::AnchorArrangement(EdgeTracer & edgeTracer) :
      tracer_(edgeTracer)
  {
    init();
  }

template< typename LabelType>
  typename AnchorArrangement< LabelType>::PointsIterator
  AnchorArrangement< LabelType>::beginPoints()
  {
    return points_.begin();
  }

template< typename LabelType>
  typename AnchorArrangement< LabelType>::PointsIterator
  AnchorArrangement< LabelType>::endPoints()
  {
    return points_.end();
  }

template< typename LabelType>
  size_t
  AnchorArrangement< LabelType>::numPoints() const
  {
    return points_.size();
  }

template< typename LabelType>
  AnchorPoint *
  AnchorArrangement< LabelType>::getPoint(const size_t index)
  {
    return &points_[index];
  }

template< typename LabelType>
  arma::mat
  AnchorArrangement< LabelType>::getPointPositions() const
  {
    arma::mat pos(2, numPoints());
    for(size_t i = 0; i < numPoints(); ++i)
      pos.col(i) = points_[i].getPos();
    return pos;
  }

template< typename LabelType>
  void
  AnchorArrangement< LabelType>::setPointPositions(const arma::mat & pos)
  {
    for(size_t i = 0; i < numPoints(); ++i)
      points_[i].setPos(pos.col(i));
  }

template< typename LabelType>
  AnchorPoint *
  AnchorArrangement< LabelType>::getAnchorPoint(
      const typename Arrangement::Vertex_const_handle & vertex)
  {
    return vertex->data().anchor;
  }

template< typename LabelType>
  const typename AnchorArrangement< LabelType>::Arrangement &
  AnchorArrangement< LabelType>::getCgalArrangement() const
  {
    return tracer_.getArrangement();
  }

template< typename LabelType>
  ::boost::optional< double>
  AnchorArrangement< LabelType>::getFaceAnchorArea(
      const typename Arrangement::Face & face) const
  {
    typedef typename Arrangement::Geometry_traits_2::Kernel Kernel;

    if(face.is_unbounded() || face.is_fictitious())
      return boost::optional< double>();

    const typename CGAL::Polygon_2< Kernel> poly = getFacePolygon(face);
    return std::fabs(CGAL::to_double(poly.area()));
  }

template< typename LabelType>
  typename AnchorArrangement< LabelType>::FacePolygon
  AnchorArrangement< LabelType>::getFacePolygon(
      const typename Arrangement::Face & face) const
  {
    SSLIB_ASSERT(!face.is_unbounded());
    SSLIB_ASSERT(!face.is_fictitious());

    typedef typename Arrangement::Geometry_traits_2::Kernel Kernel;

    FacePolygon poly;

    const typename Arrangement::Ccb_halfedge_const_circulator first =
        face.outer_ccb();
    typename Arrangement::Ccb_halfedge_const_circulator cl = first;
    const AnchorPoint * anchor;
    do
    {
      anchor = cl->source()->data().anchor;
      const arma::vec2 & anchorPos = anchor->getPos();
      poly.push_back(typename Kernel::Point_2(anchorPos(0), anchorPos(1)));
      ++cl;
    }
    while(cl != first);
    return poly;
  }

template< typename LabelType>
  void
  AnchorArrangement< LabelType>::init()
  {
    using std::pair;
    using std::make_pair;

    generateArrangementVertices();
  }

template< typename LabelType>
  void
  AnchorArrangement< LabelType>::generateArrangementVertices()
  {
    Arrangement & arrangement = tracer_.getArrangement();

    arma::vec2 pt;
    for(typename Arrangement::Vertex_iterator it = arrangement.vertices_begin(),
        end = arrangement.vertices_end(); it != end; ++it)
    {
      pt(0) = CGAL::to_double(it->point()[0]);
      pt(1) = CGAL::to_double(it->point()[1]);

      points_.push_back(
          new AnchorPoint(points_.size(), pt, it->data().maxDisplacement));
      it->data().anchor = &points_.back();
    }
  }

} // namespace analysis
} // namespace spl

#endif /* ANCHOR_POINT_ARRANGEMENT_DETAIL_H */
