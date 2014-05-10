/*
 * VoronoiEdgeTracer.h
 *
 *  Created on: Oct 15, 2013
 *      Author: Martin Uhrin
 */

#ifndef VORONOI_EDGE_TRACER_DETAIL_H
#define VORONOI_EDGE_TRACER_DETAIL_H

// INCLUDES ///////////////////
#include <map>
#include <vector>

#include <armadillo>

#include <boost/foreach.hpp>
#include <boost/range/iterator_range.hpp>

#include <CGAL/Object.h>

#include "spl/SSLibAssert.h"

// FORWARD DECLARATIONS ///////

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::VoronoiEdgeTracer(
      const Voronoi & voronoi, const bool _splitSharedVertices)
  {
    TracingData tracingData(voronoi);

    //createBoundary(&tracingData);
    initArrangement(tracingData);

    tracingData.toVisit = tracingData.boundaryEdges;
    for(typename DelaunayEdgeSet::const_reverse_iterator it =
        tracingData.toVisit.rbegin(); !tracingData.toVisit.empty(); it =
        tracingData.toVisit.rbegin())
    {
      traceEdge(tracingData, --it.base());
    }

    if(_splitSharedVertices)
      splitSharedVertices();

    splitEdges();
    populateFaceLabels();

  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  typename VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType,
      FaceDataType>::Arrangement &
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::getArrangement()
  {
    return arrangement_;
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  const typename VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType,
      FaceDataType>::Arrangement &
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::getArrangement() const
  {
    return arrangement_;
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  void
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::initArrangement(
      TracingData & tracingData)
  {
    // Fist we need to place all the vertices we're going to use into the
    // unbounded face of the arrangement as this make the subsequent step
    // of creating the edges a lot easier

    typedef ::std::pair< typename VertexMap::iterator, bool> VertexMapInsertReturn;

    typename Arrangement::Face_handle unboundedFace =
        arrangement_.unbounded_face();
    const Voronoi & voronoi = tracingData.voronoi;
    const typename Voronoi::Delaunay_graph & delaunay = voronoi.dual();

    const typename Voronoi::Adaptation_policy::Edge_rejector & edge_rejector =
        voronoi.adaptation_policy().edge_rejector_object();

    for(typename Delaunay::Edge_iterator it = delaunay.edges_begin(), end =
        delaunay.edges_end(); it != end; ++it)
    {
      if(isBoundaryEdge(*it) && !edge_rejector(delaunay, *it))
      {
        const typename Voronoi::Halfedge_handle he = voronoi.dual(*it);

        // Save this Delaunay edge as it corresponds to a boundary
        tracingData.boundaryEdges.insert(*it);

        // Populate the map arrangement with vertices if necessary
        if(he->has_source())
        {
          const VertexMapInsertReturn ret = tracingData.vertexMap.insert(
              ::std::make_pair(he->source(), arrangement_.vertices_end()));

          // Is the vertex new to the arrangement?
          if(ret.second)
          {
            ret.first->second = arrangement_.insert_in_face_interior(
                he->source()->point(), unboundedFace);
            ret.first->second->data().maxDisplacement = ::CGAL::to_double(
                ::CGAL::sqrt(
                    ::CGAL::squared_distance(ret.first->second->point(),
                        it->first->vertex((it->second + 1) % 3)->point())));
          }
        }
        if(he->has_target())
        {
          const VertexMapInsertReturn ret = tracingData.vertexMap.insert(
              ::std::make_pair(he->target(), arrangement_.vertices_end()));

          // Is the vertex new to the arrangement?
          if(ret.second)
          {
            ret.first->second = arrangement_.insert_in_face_interior(
                he->target()->point(), unboundedFace);
            ret.first->second->data().maxDisplacement = ::CGAL::to_double(
                ::CGAL::sqrt(
                    ::CGAL::squared_distance(ret.first->second->point(),
                        it->first->vertex((it->second + 1) % 3)->point())));
          }
        }
//        // If it's a boundary, create a vertex for the Voronoi edge to connect to that is
//        // at the midpoint of the dual Delaunay edge
//        if(he->is_unbounded())
//        {
//          const typename Arrangement::Vertex_handle vtx = arrangement_.insert_in_face_interior(
//              midpoint(*it), unboundedFace);
//          vtx->data().maxDisplacement = ::std::sqrt(
//              ::CGAL::squared_distance(vtx->point(),
//                  it->first->vertex((it->second + 1) % 3)->point()));
//          vtx->data().isOuterBoundary = true;
//
////          vtx->data().maxDisplacement = 0.0;
//          tracingData.boundaryVertices[he] = vtx;
//        }

      }
    }

//    //size_t i = 0;
//    typename Delaunay::Edge_circulator cl = delaunay.incident_edges(
//        delaunay.incident_vertices(delaunay.infinite_vertex()));
//    typename Delaunay::Edge_circulator first = cl;
//    do
//    {
//      if(delaunay.is_infinite(cl->first->vertex(cl->second)) &&
//          voronoi.dual(*cl)->is_unbounded())
//      {
//        //::std::cout << "Found initial edge.\n";
//        break;
//      }
//      // Move on to the next one
//      ++cl;
//    } while(cl != first);
//
//    first = cl;
//    Point r1, r2;
//    K::Vector_2 dr;
//    do
//    {
//      r1 = cl->first->vertex(delaunay.cw(cl->second))->point();
//      r2 = cl->first->vertex(delaunay.ccw(cl->second))->point();
//      dr = r2 - r1;
//
//      if(isBoundaryEdge(*cl))
//        ::std::cout << r1 << " " << dr.x() << " " << dr.y() << " 1\n";
//      else
//        ::std::cout << r1 << " " << dr.x() << " " << dr.y() << " 0\n";
//
//      const typename Delaunay::Edge_circulator starting = cl;
//      do
//      {
//        ++cl;
//        if(cl != starting && voronoi.dual(*cl)->is_unbounded())
//        {
//          cl = delaunay.incident_edges(cl->first->vertex(delaunay.ccw(cl->second)));
//          //::std::cout << "Found next edge\n";
//          break;
//        }
//      } while(true/*cl != starting*/);
//
//    } while(cl != first);
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  void
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::traceEdge(
      TracingData & tracingData,
      typename DelaunayEdgeSet::const_iterator edgeIt)
  {
    SSLIB_ASSERT(isBoundaryEdge(*edgeIt));

    typename Voronoi::Delaunay_edge edge = *edgeIt;
    const typename Voronoi::Halfedge_handle halfEdge = tracingData.voronoi.dual(
        edge);

    if(halfEdge->is_unbounded())
    {
      tracingData.toVisit.erase(edgeIt);
      return;
    }

    const typename VertexMap::const_iterator source =
        halfEdge->has_source() ?
            tracingData.vertexMap.find(halfEdge->source()) :
            tracingData.vertexMap.end();

    traceEdge(tracingData, halfEdge, source);
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  void
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::traceEdge(
      TracingData & tracingData,
      const typename Voronoi::Halfedge_handle & halfEdge,
      const typename VertexMap::const_iterator source)
  {
    typename Voronoi::Delaunay_edge delaunayEdge = halfEdge->dual();

    if(halfEdge->is_unbounded())
      return;

    if(!isBoundaryEdge(delaunayEdge))
      return; // Not boundary

    const typename DelaunayEdgeSet::const_iterator it =
        tracingData.toVisit.find(delaunayEdge);
    if(it == tracingData.toVisit.end())
      return; // Already visited

    typename VertexMap::const_iterator target = tracingData.vertexMap.end();
    typename Arrangement::Vertex_handle v1, v2;
    if(halfEdge->is_unbounded())
    {
      v1 = tracingData.boundaryVertices.find(halfEdge)->second;
      if(source == tracingData.vertexMap.end())
      {
        // The source must be the infinite vertex, so target should be a valid
        // Voronoi vertex
        target = tracingData.vertexMap.find(halfEdge->target());
        SSLIB_ASSERT(target != tracingData.vertexMap.end());

        v2 = target->second;
      }
      else
        v2 = source->second;
    }
    else
    {
      SSLIB_ASSERT(source != tracingData.vertexMap.end());

      target = tracingData.vertexMap.find(halfEdge->target());
      SSLIB_ASSERT(target != tracingData.vertexMap.end());

      v1 = source->second;
      v2 = target->second;
    }

    // Insert the segment corresponding to the edge into the arrangement
    const ArrSegment segment(v1->point(), v2->point());
    const typename Arrangement::Halfedge_handle heHandle =
        arrangement_.insert_at_vertices(segment, v1, v2);

    // Save the label either side of the halfedge
    heHandle->data().label = halfEdge->face()->dual()->info();
    heHandle->twin()->data().label = halfEdge->twin()->face()->dual()->info();

    // Finished this Delaunay edge
    tracingData.toVisit.erase(it);

    // Do the neighbouring edges
    if(target != tracingData.vertexMap.end())
      traceEdge(tracingData, halfEdge->next(), target);
    if(source != tracingData.vertexMap.end())
      traceEdge(tracingData, halfEdge->opposite()->next(), source);
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  bool
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::isBoundaryEdge(
      const typename Delaunay::Edge & edge) const
  {
    return edge.first->vertex((edge.second + 1) % 3)->info()
        != edge.first->vertex((edge.second + 2) % 3)->info();
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  void
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::createBoundary(
      TracingData * const tracingData) const
  {
    typedef typename K::Ray_2 Ray;
    typedef typename K::Segment_2 Segment;

    const Delaunay & dg = tracingData->voronoi.dual();
    typename Delaunay::Edge_circulator cl = dg.incident_edges(
        dg.infinite_vertex());
    const typename Delaunay::Edge_circulator first = cl;
    typename Delaunay::Vertex_handle v1, v2;
    typename Delaunay::Face_handle face;
    do
    {
      face = cl->first;
      v1 = face->vertex(face->ccw(cl->second));
      v2 = face->vertex(face->cw(cl->second));

      tracingData->boundary.push_back(v1->point());

      ::CGAL::Object dual = dg.dual(*cl);
      if(const Ray * const ray = ::CGAL::object_cast< Ray>(&dual))
      {
        const Segment boundary(v1->point(), v2->point());
        const ::CGAL::Object intersection = ::CGAL::intersection(boundary,
            *ray);
        if(const Point * const point = ::CGAL::object_cast< Point>(
            &intersection))
        {
          tracingData->boundary.push_back(*point);
        }
      }
      ++cl;
    }
    while(cl != first);
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  void
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::splitSharedVertices()
  {
    typedef ::std::vector<
        typename Arrangement::Halfedge_around_vertex_circulator> SurroundingZones;

    using ::std::make_pair;

    typename Arrangement::Vertex_iterator vNext;
    for(typename Arrangement::Vertex_iterator vIt =
        arrangement_.vertices_begin(), vEnd = arrangement_.vertices_end();
        vIt != vEnd; vIt = vNext)
    {
      // Store an iterator to the next vertex (as we may delete vIt and
      // invalidate the iterator).
      vNext = vIt;
      ++vNext;

      typename Arrangement::Vertex & vertex = *vIt;
      const size_t degree = vertex.degree();

      // Can't split vertices with less than 4 zones
      if(degree > 3)
      {
        const typename Arrangement::Halfedge_around_vertex_circulator first =
            vertex.incident_halfedges();
        typename Arrangement::Halfedge_around_vertex_circulator cl = first;

        // Put all the halfedges that correspond to zones into an array
        // so we can lookup using the index
        SurroundingZones zones;
        do
        {
          zones.push_back(cl);
          ++cl;
        }
        while(cl != first);

        VertexSplitter splitter(degree);
        {
          // Find the graph of connected zones
          ::std::vector< bool> visitedZones(degree, false);
          {
            size_t lastZone, numConnections;
            for(size_t i = 0; i < zones.size(); ++i)
            {
              if(!visitedZones[i])
              {
                lastZone = i;
                numConnections = 0;

                // Check counterclockwise for connections
                for(size_t j = i + 1; j < zones.size(); ++j)
                {
                  if(zones[i]->data().label == zones[j]->data().label)
                  {
                    splitter.addEdge(lastZone, j, zones[i]->data().label);
                    lastZone = j;
                    visitedZones[j] = true;
                    ++numConnections;
                  }
                }

                // Check clockwise for a wrap-around connection,
                // e.g. 1->3, 3->5, 5->7 and 7->1 (but expressed as 1->7)
                if(numConnections > 1)
                  splitter.addEdge(i, lastZone, zones[i]->data().label);

                visitedZones[i] = true;
              } // (!visitedZones[i])
            }
          }
        }

        // Get rid of colliding edges
        splitter.resolveCollisions();
        if(splitter.noEdges())
          continue;

        typedef ::std::vector< SplitVertex> NewVertices;

        ::std::vector< int> edgeVertexIndices(degree, -1);
        NewVertices vertices;

        {
          ::std::pair< int, int> zone;
          BOOST_FOREACH(typename VertexSplitter::EdgeIterator::reference entry,
              ::boost::make_iterator_range(splitter.edgesBegin(), splitter.edgesEnd()))
          {
            const typename VertexSplitter::Edge & edge = entry.first;
            {
              SplitVertex sv;

              zone.first = edge.first();
              zone.second = (edge.second() + 1) % degree;

              if(edgeVertexIndices[zone.first] == -1)
              {
                sv.addHalfedge(zones[zone.first]);
                edgeVertexIndices[zone.first] = vertices.size();
              }

              if(edgeVertexIndices[zone.second] == -1)
              {
                sv.addHalfedge(zones[zone.second]);
                edgeVertexIndices[zone.second] = vertices.size();
              }

              if(sv.numNeighbours() != 0)
                vertices.push_back(sv);
            }

            {
              SplitVertex sv;

              // Move on to the next zone
              zone.first = (edge.first() + 1) % degree;
              zone.second = edge.second();

              if(edgeVertexIndices[zone.first] == -1)
              {
                sv.addHalfedge(zones[zone.first]);
                edgeVertexIndices[zone.first] = vertices.size();
              }

              if(edgeVertexIndices[zone.second] == -1)
              {
                sv.addHalfedge(zones[zone.second]);
                edgeVertexIndices[zone.second] = vertices.size();
              }

              if(sv.numNeighbours() != 0)
                vertices.push_back(sv);
            }
          }
        }

        // Now connect up any vertices that should be at a vertex but are between zones
        {
          int zone = -1;
          // Move to the first connected edge vertex
          for(size_t k = 0; k < degree; ++k)
          {
            if(edgeVertexIndices[k] != -1
                && edgeVertexIndices[(k + 1) % degree] == -1)
            {
              zone = k;
              break;
            }
          }
          if(zone != -1)
          {
            size_t lastVertexIndex = edgeVertexIndices[zone];
            for(size_t k = 0; k < degree; ++k)
            {
              zone = (zone + 1) % degree;
              if(edgeVertexIndices[zone] == -1)
              {
                vertices[lastVertexIndex].addHalfedge(zones[zone]);
                edgeVertexIndices[zone] = lastVertexIndex;
              }
              else
                lastVertexIndex = edgeVertexIndices[zone];
            }
          }
        }

        // First remove all the old halfedges from the arrangement
        BOOST_FOREACH(typename Arrangement::Halfedge_around_vertex_circulator oldHalfedge, zones)
          arrangement_.remove_edge(oldHalfedge, false, false);

        const VertexDataType vertexData = vIt->data();
        const K::Point_2 oldPos = vIt->point();

        // Remove the old vertex
        arrangement_.remove_isolated_vertex(vIt);
        // WARNING: Don't use old vertex (vIt) or old halfedges (zones) beyond this point
        vIt = typename Arrangement::Vertex_iterator();
        zones.clear();

        BOOST_FOREACH(const SplitVertex & splitVertex, vertices)
        {
          const K::Point_2 newVertexPos = oldPos
              + 0.1 * vertexData.maxDisplacement * splitVertex.meanPos();

          // Now, create the new 'split' vertex
          const typename Arrangement::Vertex_handle newArrVertex =
              ::CGAL::insert_point(arrangement_, newVertexPos);
          newArrVertex->set_data(vertexData); // Copy the data over

          // Create the new halfedges
          size_t edgeIdx = 0;
          BOOST_FOREACH(typename Arrangement::Vertex_handle neighbour,
              ::boost::make_iterator_range(splitVertex.neighboursBegin(), splitVertex.neighboursEnd()))
          {
            // Insert the segment corresponding to the edge into the arrangement
            const ArrSegment segment(newArrVertex->point(), neighbour->point());
            const typename Arrangement::Halfedge_handle heHandle =
                arrangement_.insert_at_vertices(segment, newArrVertex,
                    neighbour);

            const typename SplitVertex::HalfedgeData & heData =
                splitVertex.getHalfedgeData(edgeIdx);
            heHandle->set_data(heData.second);
            heHandle->twin()->set_data(heData.first);

            ++edgeIdx;
          }

        }
      }
    }
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  void
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::splitEdges()
  {
    ::std::vector< typename Arrangement::Halfedge_handle> toSplit;
    for(typename Arrangement::Edge_iterator it = arrangement_.edges_begin(),
        end = arrangement_.edges_end(); it != end; ++it)
    {
      toSplit.push_back(it);
    }

    typename K::Point_2 p1, p2, midpoint;
    typename Arrangement::Vertex_handle source, mid, target;
    typename Arrangement::Halfedge_handle e1, e2;
    HalfedgeDataType e1Data, e2Data;
    while(!toSplit.empty())
    {
      const typename Arrangement::Halfedge_handle edge = toSplit.back();
      const typename Arrangement::Halfedge_handle twin = edge->twin();

      e1Data = edge->data();
      e2Data = twin->data();

      source = edge->source();
      target = edge->target();

      p1 = source->point();
      p2 = target->point();
      midpoint = ::CGAL::midpoint(p1, p2);

      const typename K::Segment_2 s1(p1, midpoint);
      const typename K::Segment_2 s2(midpoint, p2);

      const typename Arrangement::Face_handle face = arrangement_.remove_edge(
          edge, false, false);
      mid = arrangement_.insert_in_face_interior(midpoint, face);
      mid->set_data(source->data());
      e1 = arrangement_.insert_at_vertices(s1, source, mid);
      e2 = arrangement_.insert_at_vertices(s2, mid, target);

      const typename Arrangement::Halfedge_handle e1Twin = e1->twin();
      const typename Arrangement::Halfedge_handle e2Twin = e2->twin();

      e1->set_data(e1Data);
      e2->set_data(e1Data);

      e1Twin->set_data(e1Data);
      e2Twin->set_data(e2Data);

      toSplit.pop_back();
    }
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  void
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::populateFaceLabels()
  {
    BOOST_FOREACH(typename Arrangement::Face & face,
        ::boost::make_iterator_range(arrangement_.faces_begin(),
            arrangement_.faces_end()))
    {
      // Get the label from one of the halfedges
      if(!face.is_unbounded() && !face.is_fictitious())
        face.data().label = face.outer_ccb()->data().label;
    }

  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  typename VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType,
      FaceDataType>::Point
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::midpoint(
      const typename Delaunay::Edge & edge) const
  {
    return ::CGAL::midpoint(edge.first->vertex((edge.second + 1) % 3)->point(),
        edge.first->vertex((edge.second + 2) % 3)->point());
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::VertexSplitter::VertexSplitter(
      const size_t degree) :
      degree_(degree)
  {
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  void
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::VertexSplitter::addEdge(
      const size_t i, const size_t j, const LabelType & label)
  {
    edges_[Edge(i, j)] = label;
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  typename VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType,
      FaceDataType>::VertexSplitter::EdgeIterator
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::VertexSplitter::edgesBegin() const
  {
    return edges_.begin();
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  typename VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType,
      FaceDataType>::VertexSplitter::EdgeIterator
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::VertexSplitter::edgesEnd() const
  {
    return edges_.end();
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  bool
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::VertexSplitter::noEdges() const
  {
    return edges_.empty();
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  size_t
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::VertexSplitter::numEdges() const
  {
    return edges_.size();
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  void
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::VertexSplitter::resolveCollisions()
  {
    using ::std::make_pair;

    ::std::vector<
        ::std::vector< ::std::vector< ::std::pair< Edge, LabelType> > > > collisionMatrix(
        degree_);
    // Create a lower triangular matrix of Edges
    for(int i = 0; i < degree_; ++i)
      collisionMatrix[i].resize(i + 1);

    // Perform edge collision detection
    BOOST_FOREACH(typename Edges::const_reference entry, edges_)
    {
      for(size_t i = entry.first.first(); i <= entry.first.second(); ++i)
      {
        // Sweep the edge square
        // bottom left to bottom right
        collisionMatrix[entry.first.second()][i].push_back(entry);
        // top left to bottom right
        collisionMatrix[i][entry.first.first()].push_back(entry);
      }
    }

    ::std::set< Edge> toRemove;
    // Now all the matrix entries with two or more edges that don't have a common
    // label will be considered 'colliding' and should be removed
    for(int i = 0; i < degree_; ++i)
    {
      for(int j = 0; j < i; ++j)
      {
        if(collisionMatrix[i][j].size() > 1)
        {
          // Test all pairs of edges for different edge labels
          for(int k = 0; k < collisionMatrix[i][j].size(); ++k)
          {
            for(int l = k + 1; l < collisionMatrix[i][j].size(); ++l)
            {
              const LabelType l1 = collisionMatrix[i][j][k].second;
              const LabelType l2 = collisionMatrix[i][j][l].second;
              if(l1 != l2)
              {
                toRemove.insert(collisionMatrix[i][j][k].first);
                toRemove.insert(collisionMatrix[i][j][l].first);
              }
            }
          }
        }
      }
    }

    BOOST_FOREACH(const Edge & edge, toRemove)
      edges_.erase(edge);
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::SplitVertex::SplitVertex() :
      newPos_(::CGAL::NULL_VECTOR)
  {
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  void
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::SplitVertex::addHalfedge(
      const typename Arrangement::Halfedge_around_vertex_circulator & halfedge)
  {
    halfedges_.push_back(halfedge);
    neighbours_.push_back(halfedge->source());
    halfedgeData_.push_back(
        HalfedgeData(halfedge->data(), halfedge->twin()->data()));
    const K::Vector_2 dr = halfedge->source()->point()
        - halfedge->target()->point();
    newPos_ = newPos_ + dr / ::CGAL::sqrt(dr.squared_length());
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  size_t
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::SplitVertex::numNeighbours() const
  {
    return neighbours_.size();
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  typename VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType,
      FaceDataType>::SplitVertex::NeighbourIterator
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::SplitVertex::neighboursBegin() const
  {
    return neighbours_.begin();
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  typename VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType,
      FaceDataType>::SplitVertex::NeighbourIterator
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::SplitVertex::neighboursEnd() const
  {
    return neighbours_.end();
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  typename VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType,
      FaceDataType>::SplitVertex::HalfedgeIterator
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::SplitVertex::halfedgesBegin() const
  {
    return halfedges_.begin();
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  typename VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType,
      FaceDataType>::SplitVertex::HalfedgeIterator
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::SplitVertex::halfedgesEnd() const
  {
    return halfedges_.end();
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  const typename VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType,
      FaceDataType>::SplitVertex::HalfedgeData &
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::SplitVertex::getHalfedgeData(
      const size_t idx) const
  {
    return halfedgeData_[idx];
  }

template< typename LabelType, class VertexDataType, class HalfedgeDataType,
    class FaceDataType>
  typename VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType,
      FaceDataType>::SplitVertex::Vector_2
  VoronoiEdgeTracer< LabelType, VertexDataType, HalfedgeDataType, FaceDataType>::SplitVertex::meanPos() const
  {
    return newPos_ / neighbours_.size();
  }
}
}

#endif /* VORONOI_EDGE_TRACER_DETAIL_H */
