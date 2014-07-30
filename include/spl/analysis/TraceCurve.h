/*
 * TraceCurve.h
 *
 *  Created on: April 23, 2014
 *      Author: Martin Uhrin
 */

#ifndef TRACE_CURVE_H
#define TRACE_CURVE_H

// INCLUDES ////////////
#include "spl/SSLib.h"

#ifdef SPL_USE_CGAL

#include <vector>

// FORWARD DECLARATIONS ///////

// DEFINITION ///////////////////////

namespace spl {
namespace analysis {

template< typename VD>
  class TraceCurve
  {
    typedef typename VD::Delaunay_geom_traits Kernel;
    typedef typename Kernel::Point_2 Point;
  public:
    class Vertex;

    void
    pushBack(const Point & point);
    size_t
    numVertices() const;
    const Vertex &
    vertexFront() const;
    Vertex &
    vertexFront();
    const Vertex &
    vertexBack() const;
    Vertex &
    vertexBack();
    const Vertex &
    vertex(const size_t i) const;
    Vertex &
    vertex(const size_t i);

  private:
    std::vector< Vertex> myVertices;
  };

}
}

#include "spl/analysis/detail/TraceCurve.h"

#endif /* SPL_USE_CGAL */
#endif /* TRACE_CURVE_H */
