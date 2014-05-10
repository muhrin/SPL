/*
 * ArrangementSmoothing.h
 *
 *  Created on: Nov 4, 2013
 *      Author: Martin Uhrin
 */

#ifndef ARRANGEMENT_SMOOTHING_DETAIL_H
#define ARRANGEMENT_SMOOTHING_DETAIL_H

// INCLUDES ///////////////////
#include "spl/SSLibAssert.h"

#include <limits>
#include <map>

#include <boost/circular_buffer.hpp>
#include <boost/foreach.hpp>
#include <boost/range/iterator_range.hpp>

#include <armadillo>

#include <spl/analysis/AnchorArrangement.h>
#include <spl/common/Constants.h>

// FORWARD DECLARATIONS ///////

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {

template< typename LabelType>
  class ArrangementSmoother
  {
    typedef AnchorArrangement< LabelType> Arrangement;
  public:
    ArrangementSmoother(const SmoothingOptions & options,
        Arrangement * const arr);
    void
    smooth();

  private:
    typedef typename Arrangement::Arrangement CgalArrangement;
    typedef typename CgalArrangement::Face Face;
    typedef std::map< const Face *, double> FaceAreas;
    typedef boost::circular_buffer< double> TpsdHistory;

    static const size_t TPSD_FORCE_HISTORY_LENGTH = 5;
    static const double INITIAL_STEPSIZE = 1e-6;
    static const double MAX_DISPLACEMENT_THRESHOLD_SQ = 0.98;

    template< typename T>
      double
      sgn(T val)
      {
        return (T(0) < val) - (val < T(0));
      }
    void
    initFaces();
    void
    evaluateForces();
    arma::vec2
    maxDisplacementForce(const AnchorPoint & point,
        const arma::vec2 & pos) const;
    arma::vec2
    meetingVertexForce(const typename CgalArrangement::Vertex & vertex,
        const arma::vec2 & pos) const;
    void
    applySmoothingForce(const typename CgalArrangement::Vertex & vertex,
        const arma::vec2 & pos);
    void
    applySmoothingForce2(const typename CgalArrangement::Vertex & vertex,
        const arma::vec2 & pos);
    void
    applyAreaForce(typename FaceAreas::const_reference face);
    double
    maxDeviation(const TpsdHistory & values);
    arma::vec2
    perp(const arma::vec2 & r) const;

    const double smoothingForceStrength_;
    const double areaForceStrength_;
    const double vertexForceStrength_;
    const int maxSteps_;
    const double forceTol_;
    Arrangement * const arr_;
    FaceAreas faceAreas_;
    arma::mat forces_;
    arma::mat pos_;
  };

template< typename LabelType>
  ArrangementSmoother< LabelType>::ArrangementSmoother(
      const SmoothingOptions & options, Arrangement * const arr) :
      smoothingForceStrength_(options.smoothingForceStrength), areaForceStrength_(
          options.areaForceStrength), vertexForceStrength_(
          options.vertexForceStrength), maxSteps_(
          options.maxSteps == -1 ?
              std::numeric_limits< int>::max() : options.maxSteps), forceTol_(
          options.forceTol), arr_(arr), forces_(2, arr->numPoints()), pos_(2,
          arr->numPoints())
  {
    SSLIB_ASSERT(arr);
    initFaces();
  }

template< typename LabelType>
  void
  ArrangementSmoother< LabelType>::smooth()
  {
    using arma::accu;
    using std::fabs;

    arma::mat f0(2, arr_->numPoints());
    arma::mat deltaForce(2, arr_->numPoints()), deltaPos(2, arr_->numPoints());

    // Set up initial conditions
    pos_ = arr_->getPointPositions();
    deltaPos.zeros();

    evaluateForces(); // Sets forces_ to current forces
    f0 = forces_;

    TpsdHistory forceSqHistory(TPSD_FORCE_HISTORY_LENGTH);

    double forceSq;
    double xg, gg, stepsize = INITIAL_STEPSIZE;
    bool usingTpsd = true;
    size_t numSteepestIters = 0;

    for(int iter = 0; iter < maxSteps_; ++iter)
    {
      deltaPos = stepsize * forces_;
      pos_ += deltaPos;
      arr_->setPointPositions(pos_);

      // Find forces for new positions, saving the old
      f0 = forces_;
      evaluateForces();

      // Check termination criterion
      forceSq = accu(forces_ % forces_);
      if(forceSq < forceTol_)
        break;

      if(usingTpsd)
      {
        forceSqHistory.push_back(forceSq);

        // Check if we should switch away from tpsd
        if(forceSqHistory.size() == TPSD_FORCE_HISTORY_LENGTH
            && maxDeviation(forceSqHistory) < 1e-4)
        {
          stepsize = INITIAL_STEPSIZE;
          usingTpsd = false;
        }
        else
        {
          deltaForce = forces_ - f0;

          xg = accu(deltaPos % deltaForce);
          gg = accu(deltaForce % deltaForce);

          // Set the new stepsize
          if(fabs(xg) > 0.0)
            stepsize = fabs(xg / gg);
        }
      }
      else if(++numSteepestIters > 100)
      {
        // Switch TPSD back on
        numSteepestIters = 0;
        usingTpsd = true;
      }
    }
  }

template< typename LabelType>
  void
  ArrangementSmoother< LabelType>::initFaces()
  {
    const typename Arrangement::Arrangement & cgalArr =
        arr_->getCgalArrangement();

    BOOST_FOREACH(const Face & face,
        boost::make_iterator_range(cgalArr.faces_begin(), cgalArr.faces_end()))
    {
      // Only bounded, non-fictitious, faces have an area
      const boost::optional< double> area = arr_->getFaceAnchorArea(face);
      if(area)
        faceAreas_[&face] = *area;
    }
  }

template< typename LabelType>
  void
  ArrangementSmoother< LabelType>::evaluateForces()
  {
    forces_.zeros();

    arma::vec2 pos, force;
    for(typename CgalArrangement::Vertex_const_iterator it =
        arr_->getCgalArrangement().vertices_begin(), end =
        arr_->getCgalArrangement().vertices_end(); it != end; ++it)
    {
      AnchorPoint * const point = it->data().anchor;
      const size_t idx = point->idx();

      force.zeros();
      pos = point->getPos();

      if(it->degree() > 2)
      {
        // Move the connecting vertex to the midpoint of the vertices
        // that connect to it
        force += meetingVertexForce(*it, pos);
      }
      else if(it->degree() == 2)
      {
        applySmoothingForce(*it, pos);
      }

      force += maxDisplacementForce(*point, pos);

      forces_.col(idx) += force;
    }

//    double len;
//    for(typename Arrangement::Arrangement::Edge_const_iterator edge =
//        arr_->getCgalArrangement().edges_begin(), edgesEnd =
//        arr_->getCgalArrangement().edges_end(); edge != edgesEnd; ++edge)
//    {
//      const AnchorPoint * const n1 = edge->source()->data().anchor;
//      const AnchorPoint * const n2 = edge->target()->data().anchor;
//      pos = n2->getPos() - n1->getPos();
//      len = std::sqrt(arma::dot(pos, pos));
//      force = 10.0 / len * pos;
//      if(!edge->source()->data().isOuterBoundary)
//        forces_.col(n1->idx()) += force;
//      if(!edge->target()->data().isOuterBoundary)
//      forces_.col(n2->idx()) -= force;
//    }

    BOOST_FOREACH(typename FaceAreas::reference face, faceAreas_)
    {
      applyAreaForce(face);
    }
  }

template< typename LabelType>
  arma::vec2
  ArrangementSmoother< LabelType>::maxDisplacementForce(
      const AnchorPoint & point, const arma::vec2 & pos) const
  {
    const arma::vec2 dr = pos - point.getAnchorPos();

    const double lenSq = arma::dot(dr, dr);
    const double cutoffSq = MAX_DISPLACEMENT_THRESHOLD_SQ
        * point.getMaxDisplacement() * point.getMaxDisplacement();
    const double repulsionDistSq = lenSq - cutoffSq;

    if(repulsionDistSq > 0.0)
      return -1000000.0 * std::sqrt(repulsionDistSq / lenSq) * dr;

    return arma::zeros(2);
  }

template< typename LabelType>
  arma::vec2
  ArrangementSmoother< LabelType>::meetingVertexForce(
      const typename CgalArrangement::Vertex & vertex,
      const arma::vec2 & pos) const
  {
    if(vertexForceStrength_ == 0.0)
      return arma::zeros(2);

    arma::vec2 centre;
    // Circular over neighbours
    typename CgalArrangement::Halfedge_around_vertex_const_circulator cl =
        vertex.incident_halfedges();
    const typename CgalArrangement::Halfedge_around_vertex_const_circulator first =
        cl;
    do
    {
      centre += cl->source()->data().anchor->getPos();
      ++cl;
    }
    while(cl != first);

    return vertexForceStrength_
        * (centre / static_cast< float>(vertex.degree()) - pos);
  }

template< typename LabelType>
  void
  ArrangementSmoother< LabelType>::applySmoothingForce(
      const typename CgalArrangement::Vertex & vertex, const arma::vec2 & pos)
  {
    SSLIB_ASSERT(vertex.degree() == 2);

    if(smoothingForceStrength_ == 0.0)
      return;

    arma::vec2 r1Perp, r2Perp;
    const size_t vtxIdx = vertex.data().anchor->idx();

    // Set up circulators
    const typename CgalArrangement::Halfedge_around_vertex_const_circulator cl1 =
        vertex.incident_halfedges();
    typename CgalArrangement::Halfedge_around_vertex_const_circulator cl2 = cl1;
    ++cl2;

    // Find a suitable pair for the first edge
    const size_t c1Idx = cl1->source()->data().anchor->idx();
    const arma::vec2 r1 = pos_.col(c1Idx) - pos;
    const double len1 = std::sqrt(arma::dot(r1, r1));
    if(len1 == 0.0)
      return;

    // Get the perpendicular unit vector
    r1Perp(0) = r1(1) / len1;
    r1Perp(1) = -r1(0) / len1;

    const size_t c2Idx = cl2->source()->data().anchor->idx();
    const arma::vec2 r2 = pos_.col(c2Idx) - pos;

    const double len2 = std::sqrt(arma::dot(r2, r2));
    if(len2 == 0.0)
      return;

    // Get the perpendicular unit vector
    r2Perp(0) = r2(1) / len2;
    r2Perp(1) = -r2(0) / len2;

    // Have to do this check so acos doesn't get a number that is ever
    // so slightly out of the allowed range [-1:1]
    double dp = arma::dot(r1, r2) / (len1 * len2);
    dp = std::min(dp, 1.0);
    dp = std::max(dp, -1.0);

    // Use the sign from the cross product
    const double k = sgn(r1(0) * r2(1) - (r1(1) * (r2(0))));

    const double theta = std::acos(dp);
    const double torque = k * -smoothingForceStrength_
        * (theta - common::constants::PI);

    const arma::vec2 f1 = (torque / len1) * r1Perp;
    const arma::vec2 f2 = (torque / len2) * r2Perp;

    forces_.col(c1Idx) += f1;
    forces_.col(c2Idx) -= f2;

    forces_.col(vtxIdx) += f2 - f1;
  }

template< typename LabelType>
  void
  ArrangementSmoother< LabelType>::applySmoothingForce2(
      const typename CgalArrangement::Vertex & vertex, const arma::vec2 & pos)
  {
    SSLIB_ASSERT(vertex.degree() == 2);

    if(smoothingForceStrength_ == 0.0)
      return;

    arma::vec2 r1Perp, r2Perp;
    const size_t vtxIdx = vertex.data().anchor->idx();

    // Set up circulators
    const typename CgalArrangement::Halfedge_around_vertex_const_circulator cl1 =
        vertex.incident_halfedges();
    typename CgalArrangement::Halfedge_around_vertex_const_circulator cl2 = cl1;
    ++cl2;

    // Find a suitable pair for the first edge
    const size_t c1Idx = cl1->source()->data().anchor->idx();
    const arma::vec2 r1 = pos_.col(c1Idx);

    const size_t c2Idx = cl2->source()->data().anchor->idx();
    const arma::vec2 r2 = pos_.col(c2Idx);

    const arma::vec2 target = 0.5 * (r2 - r1) + r1 - pos_.col(vtxIdx);
    const double len = sqrt(arma::dot(target, target));

    if(len == 0.0)
      return;

    forces_.col(vtxIdx) += (smoothingForceStrength_ / len) * target;
  }

template< typename LabelType>
  void
  ArrangementSmoother< LabelType>::applyAreaForce(
      typename FaceAreas::const_reference face)
  {
    if(areaForceStrength_ == 0.0)
      return;

    const double areaDiff =
        (*arr_->getFaceAnchorArea(*face.first) - face.second) / face.second;

    const AnchorPoint * anchor, *n1, *n2;
    arma::vec2 dr, drPerp;
    double lenSq, len;
    const typename CgalArrangement::Ccb_halfedge_const_circulator first =
        face.first->outer_ccb();
    typename CgalArrangement::Ccb_halfedge_const_circulator edge1 = first,
        edge2 = first;
    do
    {
      ++edge2;

      // The halfedges supplied go around the boundary in the counterclockwise
      // direction
      n1 = edge1->source()->data().anchor;
      anchor = edge1->target()->data().anchor;
      n2 = edge2->target()->data().anchor;
      dr = pos_.col(n2->idx()) - pos_.col(n1->idx());
      lenSq = arma::dot(dr, dr);
      if(lenSq > 0)
      {
        len = std::sqrt(lenSq);
        // This gives a vector pointing OUT of the face
        drPerp(0) = dr(1) / len;
        drPerp(1) = -dr(0) / len;
        forces_.col(anchor->idx()) -= areaForceStrength_ * areaDiff * drPerp;
      }

      edge1 = edge2;
    }
    while(edge1 != first);
  }

template< typename LabelType>
  double
  ArrangementSmoother< LabelType>::maxDeviation(const TpsdHistory & values)
  {
    if(values.empty())
      return std::numeric_limits< double>::max();

    double mean = 0.0;
    BOOST_FOREACH(const double val, values)
      mean += val;
    mean /= static_cast< double>(values.size());

    double maxDev = 0.0;
    BOOST_FOREACH(const double val, values)
      maxDev = std::max(maxDev, std::fabs(mean - val));

    return maxDev;
  }

template< typename LabelType>
  arma::vec2
  ArrangementSmoother< LabelType>::perp(const arma::vec2 & r) const
  {
    arma::vec2 perp;
    perp(0) = r(1);
    perp(1) = -r(0);
    return perp;
  }

template< typename LabelType>
  void
  smoothArrangement(const SmoothingOptions & options,
      AnchorArrangement< LabelType> * const arr)
  {
    ArrangementSmoother< LabelType> smoother(options, arr);
    smoother.smooth();
  }

} // namespace analysis
} // namespace spl

#endif /* ARRANGEMENT_SMOOTHING_DETAIL_H */
