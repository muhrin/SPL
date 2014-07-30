/*
 * VoronoiPathArrangement.h
 *
 *  Created on: Mar 20, 2014
 *      Author: Martin Uhrin
 */

#ifndef VORONOI_PATH_ARRANGEMENT_DETAIL_H
#define VORONOI_PATH_ARRANGEMENT_DETAIL_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_USE_CGAL

#include <boost/foreach.hpp>

// FORWARD DECLARATIONS ///////

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {

template< typename VD>
  class VoronoiPathArrangement< VD>::VertexConstHandle
  {
  public:
    VertexConstHandle() :
        myPath(NULL), myVtxIndex(0)
    {
    }
    VertexConstHandle(const Path & path, const size_t vtxIndex) :
        myPath(&path), myVtxIndex(vtxIndex)
    {
    }
    size_t
    idx() const
    {
      return myVtxIndex;
    }
    const Path *
    path() const
    {
      return myPath;
    }
    const typename Path::Vertex *
    operator ->() const
    {
      return &myPath->vertex(myVtxIndex);
    }
    const typename Path::Vertex &
    operator *() const
    {
      return myPath->vertex(myVtxIndex);
    }
  private:
    const Path * myPath;
    size_t myVtxIndex;
  };

template< typename VD>
  class VoronoiPathArrangement< VD>::VertexHandle
  {
  public:
    VertexHandle() :
        myPath(NULL), myVtxIndex(0)
    {
    }
    VertexHandle(Path * const path, const size_t vtxIndex) :
        myPath(path), myVtxIndex(vtxIndex)
    {
    }
    size_t
    idx() const
    {
      return myVtxIndex;
    }
    Path *
    path() const
    {
      return myPath;
    }
    typename Path::Vertex *
    operator ->() const
    {
      return &myPath->vertex(myVtxIndex);
    }
    typename Path::Vertex &
    operator *() const
    {
      return myPath->vertex(myVtxIndex);
    }
  private:
    Path * myPath;
    size_t myVtxIndex;
  };

template< typename VD>
  VoronoiPathArrangement< VD>::VoronoiPathArrangement(const Voronoi & voronoi) :
      myVoronoi(voronoi)
  {
  }

template< typename VD>
  typename VoronoiPathArrangement< VD>::PathIterator
  VoronoiPathArrangement< VD>::insertPath(const Path & path)
  {
    SSLIB_ASSERT(&myVoronoi == path.getVoronoi());

    if(path.empty())
      return myPaths.end();

    return myPaths.insert(myPaths.end(), path);
  }

template< typename VD>
  typename VoronoiPathArrangement< VD>::PathIterator
  VoronoiPathArrangement< VD>::pathsBegin()
  {
    return myPaths.begin();
  }

template< typename VD>
  typename VoronoiPathArrangement< VD>::PathIterator
  VoronoiPathArrangement< VD>::pathsEnd()
  {
    return myPaths.end();
  }

template< typename VD>
  typename VoronoiPathArrangement< VD>::PathConstIterator
  VoronoiPathArrangement< VD>::pathsBegin() const
  {
    return myPaths.begin();
  }

template< typename VD>
  typename VoronoiPathArrangement< VD>::PathConstIterator
  VoronoiPathArrangement< VD>::pathsEnd() const
  {
    return myPaths.end();
  }

template< typename VD>
  typename VoronoiPathArrangement< VD>::MeetingVertices
  VoronoiPathArrangement< VD>::getMeetingVertices()
  {
    MeetingVertices meeting;
    BOOST_FOREACH(Path & path, myPaths)
    {
      // Keep track of the path endpoints so we know the meeting points of
      // paths in the arrangement
      if(!path.isClosed())
      {
        const typename Voronoi::Vertex_handle startVtx =
            path.vertex(0).voronoiVertex();
        if(startVtx != typename Voronoi::Vertex_handle())
        {
          meeting.insert(std::make_pair(startVtx, VertexHandle(&path, 0)));
        }
        const typename Voronoi::Vertex_handle endVtx = path.vertex(
            path.numVertices() - 1).voronoiVertex();
        if(endVtx != typename Voronoi::Vertex_handle())
        {
          meeting.insert(
              std::make_pair(endVtx,
                  VertexHandle(&path, path.numVertices() - 1)));
        }
      }
    }
    return meeting;
  }

template< typename VD>
  typename VoronoiPathArrangement< VD>::MeetingVerticesConst
  VoronoiPathArrangement< VD>::getMeetingVertices() const
  {
    MeetingVerticesConst meeting;
    BOOST_FOREACH(const Path & path, myPaths)
    {
      // Keep track of the path endpoints so we know the meeting points of
      // paths in the arrangement
      if(!path.isClosed())
      {
        const typename Voronoi::Vertex_handle startVtx =
            path.vertex(0).voronoiVertex();
        if(startVtx != typename Voronoi::Vertex_handle())
        {
          meeting.insert(std::make_pair(startVtx, VertexConstHandle(path, 0)));
        }
        const typename Voronoi::Vertex_handle endVtx = path.vertex(
            path.numVertices() - 1).voronoiVertex();
        if(endVtx != typename Voronoi::Vertex_handle())
        {
          meeting.insert(
              std::make_pair(endVtx,
                  VertexConstHandle(path, path.numVertices() - 1)));
        }
      }
    }
    return meeting;
  }

template< typename VD>
  typename VoronoiPathArrangement< VD>::BoundaryVertices
  VoronoiPathArrangement< VD>::getBoundaryVertices()
  {
    const Delaunay & delaunay = myVoronoi.dual();

    BoundaryVertices boundary;
    BOOST_FOREACH(Path & path, myPaths)
    {
      // Keep track of the path endpoints so we know the meeting points of
      // paths in the arrangement
      if(!path.isClosed())
      {
        const size_t n = path.numVertices();

        // Check start vertex
        if(path.vertexFront().isBoundary())
        {
          typename Delaunay::Edge e = path.vertexFront().getBoundaryEdge();
          if(!delaunay.is_infinite(e.first))
            e = delaunay.mirror_edge(e);

          SSLIB_ASSERT(delaunay.is_infinite(e.first));
          boundary.insert(std::make_pair(e, VertexHandle(&path, 0)));
        }

        // Check end vertex
        if(n > 1 && path.vertexBack().isBoundary())
        {
          typename Delaunay::Edge e = path.vertexBack().getBoundaryEdge();
          if(!delaunay.is_infinite(e.first))
            e = delaunay.mirror_edge(e);

          SSLIB_ASSERT(delaunay.is_infinite(e.first));
          boundary.insert(std::make_pair(e, VertexHandle(&path, n - 1)));
        }
      }
    }
    return boundary;
  }

template< typename VD>
  typename VoronoiPathArrangement< VD>::BoundaryVerticesConst
  VoronoiPathArrangement< VD>::getBoundaryVertices() const
  {
    const Delaunay & delaunay = myVoronoi.dual();

    BoundaryVerticesConst boundary;
    BOOST_FOREACH(const Path & path, myPaths)
    {
      // Keep track of the path endpoints so we know the meeting points of
      // paths in the arrangement
      if(!path.isClosed())
      {
        const size_t n = path.numVertices();

        // Check start vertex
        if(path.vertexFront().isBoundary())
        {
          typename Delaunay::Edge e = path.vertexFront().getBoundaryEdge();
          if(!delaunay.is_infinite(e.first))
            e = delaunay.mirror_edge(e);

          SSLIB_ASSERT(delaunay.is_infinite(e.first));
          boundary.insert(std::make_pair(e, VertexConstHandle(path, 0)));
        }

        // Check end vertex
        if(n > 1 && path.vertexBack().isBoundary())
        {
          typename Delaunay::Edge e = path.vertexBack().getBoundaryEdge();
          if(!delaunay.is_infinite(e.first))
            e = delaunay.mirror_edge(e);

          SSLIB_ASSERT(delaunay.is_infinite(e.first));
          boundary.insert(std::make_pair(e, VertexConstHandle(path, n - 1)));
        }
      }
    }
    return boundary;
  }

template< typename VD>
  void
  VoronoiPathArrangement< VD>::print() const
  {
    BOOST_FOREACH(const Path & p, myPaths)
    {
      BOOST_FOREACH(const typename Path::Vertex & v,
          boost::make_iterator_range(p.verticesBegin(), p.verticesEnd()))
        std::cout << v.point() << "\n";
      std::cout << "\n" << std::endl;
    }
  }

}
}

#endif /* SPL_USE_CGAL */
#endif /* VORONOI_PATH_ARRANGEMENT_DETAIL_H */
