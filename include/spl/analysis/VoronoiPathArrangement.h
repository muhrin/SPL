/*
 * VoronoiPathArrangement.h
 *
 *  Created on: Mar 20, 2014
 *      Author: Martin Uhrin
 */

#ifndef VORONOI_PATH_ARRANGEMENT_H
#define VORONOI_PATH_ARRANGEMENT_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_WITH_CGAL

#include <vector>

#include "spl/analysis/VoronoiPath.h"

// FORWARD DECLARATIONS ///////

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {

template< typename VD>
  class VoronoiPathArrangement
  {
    typedef VD Voronoi;
    typedef typename Voronoi::Delaunay_graph Delaunay;
  public:
    class VertexHandle;
    class VertexConstHandle;

    typedef VoronoiPath< Voronoi> Path;
//    typedef std::pair< Path *, size_t> VertexHandle;
//    typedef std::pair< const Path *, size_t> VertexConstHandle;
    typedef typename std::vector< Path>::iterator PathIterator;
    typedef typename std::vector< Path>::const_iterator PathConstIterator;
    typedef std::multimap< typename Voronoi::Vertex_handle, VertexHandle> MeetingVertices;
    typedef std::multimap< typename Voronoi::Vertex_handle, VertexConstHandle> MeetingVerticesConst;
    typedef std::multimap< typename Delaunay::Edge, VertexHandle> BoundaryVertices;
    typedef std::multimap< typename Delaunay::Edge, VertexConstHandle> BoundaryVerticesConst;

    VoronoiPathArrangement(const Voronoi & voronoi);

    PathIterator
    insertPath(const Path & path);

    PathIterator
    pathsBegin();
    PathIterator
    pathsEnd();

    PathConstIterator
    pathsBegin() const;
    PathConstIterator
    pathsEnd() const;

    MeetingVertices
    getMeetingVertices();
    MeetingVerticesConst
    getMeetingVertices() const;

    BoundaryVertices
    getBoundaryVertices();
    BoundaryVerticesConst
    getBoundaryVertices() const;

    size_t
    numPaths() const
    {
      return myPaths.size();
    }

    void
    print() const;

    const Voronoi &
    getVoronoi() const
    {
      return myVoronoi;
    }

  private:
    struct CommonVoronoiVertices
    {
      enum Value
      {
        FORWARDS, BACKWARDS, NEITHER
      };
    };

    typename CommonVoronoiVertices::Value
    commonVoronoiVertices(const Path & p1, const Path & p2) const;

    const Voronoi & myVoronoi;
    std::vector< Path> myPaths;
  };

}
}

#include "spl/analysis/detail/VoronoiPathArrangement.h"

#endif /* SPL_WITH_CGAL */
#endif /* VORONOI_PATH_ARRANGEMENT_H */
